import crypto from "node:crypto";
import fs from "node:fs/promises";
import path from "node:path";
import { DatabaseSync } from "node:sqlite";
import express from "express";

const port = 3000;
const databaseFile = process.env.DB_FILE ?? "/data/payloads.sqlite";
const legacyDataFile = process.env.DATA_FILE ?? "/data/payloads.json";
const bearerToken = process.env.API_BEARER_TOKEN ?? "";
const publicDir = path.resolve("public");
const activity = [];

await fs.mkdir(path.dirname(databaseFile), { recursive: true });
const databaseAlreadyExists = await fs.access(databaseFile).then(() => true, () => false);
const database = new DatabaseSync(databaseFile);
database.exec(`
  PRAGMA journal_mode = WAL;
  PRAGMA synchronous = NORMAL;
  CREATE TABLE IF NOT EXISTS payloads (
    id TEXT PRIMARY KEY,
    topic TEXT NOT NULL,
    payload_json TEXT NOT NULL,
    created_at TEXT NOT NULL,
    updated_at TEXT NOT NULL
  );
  CREATE INDEX IF NOT EXISTS payloads_created_at_idx ON payloads(created_at DESC);
  CREATE INDEX IF NOT EXISTS payloads_topic_idx ON payloads(topic);
`);

const statements = {
  count: database.prepare("SELECT COUNT(*) AS count FROM payloads"),
  get: database.prepare("SELECT * FROM payloads WHERE id = ?"),
  list: database.prepare("SELECT * FROM payloads ORDER BY created_at DESC LIMIT ? OFFSET ?"),
  insert: database.prepare(`
    INSERT INTO payloads (id, topic, payload_json, created_at, updated_at)
    VALUES (?, ?, ?, ?, ?)
  `),
  update: database.prepare(`
    UPDATE payloads
    SET topic = ?, payload_json = ?, updated_at = ?
    WHERE id = ?
  `),
  delete: database.prepare("DELETE FROM payloads WHERE id = ?"),
};

function record(source, action, detail = {}) {
  activity.unshift({
    id: crypto.randomUUID(),
    timestamp: new Date().toISOString(),
    source,
    action,
    detail,
  });
  activity.splice(200);
}

function identifyCaller(req) {
  const forwardedFor = req.get("x-forwarded-for")?.split(",")[0]?.trim();
  return {
    deviceId: req.get("x-device-id") ?? null,
    clientName: req.get("x-client-name") ?? null,
    ip: forwardedFor ?? req.ip,
    userAgent: req.get("user-agent") ?? null,
  };
}

function requestDetails(req, status) {
  return {
    ...identifyCaller(req),
    method: req.method,
    path: req.path,
    status,
    authenticated: Boolean(req.apiAuthenticated),
  };
}

function isDashboardRequest(req) {
  if (req.get("x-monitor-request") === "dashboard") return true;
  const referer = req.get("referer");
  if (!referer) return false;
  try {
    return new URL(referer).host === req.get("host");
  } catch {
    return false;
  }
}

function deserializePayload(row) {
  if (!row) return null;
  return {
    id: row.id,
    topic: row.topic,
    payload: JSON.parse(row.payload_json),
    createdAt: row.created_at,
    updatedAt: row.updated_at,
  };
}

async function migrateLegacyPayloads() {
  if (databaseAlreadyExists) return;
  try {
    const payloads = JSON.parse(await fs.readFile(legacyDataFile, "utf8"));
    if (Array.isArray(payloads) && payloads.length > 0) {
      database.exec("BEGIN");
      try {
        for (const payload of payloads) {
          statements.insert.run(
            payload.id,
            payload.topic,
            JSON.stringify(payload.payload),
            payload.createdAt,
            payload.updatedAt,
          );
        }
        database.exec("COMMIT");
      } catch (error) {
        database.exec("ROLLBACK");
        throw error;
      }
    }
    await fs.unlink(legacyDataFile);
  } catch (error) {
    if (error.code !== "ENOENT") throw error;
  }
}

function parsePageInteger(value, fallback, maximum) {
  const parsed = Number.parseInt(value, 10);
  if (!Number.isFinite(parsed) || parsed < 0) return fallback;
  return Math.min(parsed, maximum);
}

function requireBearer(req, res, next) {
  if (!bearerToken) {
    return res.status(503).json({ error: "API_BEARER_TOKEN is not configured" });
  }
  const supplied = req.get("authorization")?.replace(/^Bearer\s+/i, "") ?? "";
  const expected = Buffer.from(bearerToken);
  const actual = Buffer.from(supplied);
  if (expected.length !== actual.length || !crypto.timingSafeEqual(expected, actual)) {
    return res.status(401).json({ error: "A valid bearer token is required" });
  }
  req.apiAuthenticated = true;
  next();
}

const app = express();
app.disable("x-powered-by");
app.use(express.json({ limit: "64kb" }));
app.use((req, res, next) => {
  const hasNamedCaller = Boolean(req.get("x-device-id") || req.get("x-client-name"));
  const isRelevantRequest = req.method !== "GET" || hasNamedCaller;
  if (
    req.path.startsWith("/api/") &&
    isRelevantRequest &&
    !isDashboardRequest(req) &&
    !["/api/activity", "/api/health"].includes(req.path)
  ) {
    res.on("finish", () => {
      record("api", "request", {
        ...requestDetails(req, res.statusCode),
        ...res.locals.activityDetail,
      });
    });
  }
  next();
});

app.get("/api/health", (_req, res) => {
  res.json({ status: "ok", timestamp: new Date().toISOString() });
});

app.get("/api/payloads", (req, res) => {
  const limit = parsePageInteger(req.query.limit, 100, 1000);
  const offset = parsePageInteger(req.query.offset, 0, Number.MAX_SAFE_INTEGER);
  const total = statements.count.get().count;
  const payloads = statements.list.all(limit, offset).map(deserializePayload);
  res.set("X-Total-Count", String(total));
  res.json(payloads);
});

app.get("/api/payloads/count", (_req, res) => {
  res.json({ count: statements.count.get().count });
});

app.get("/api/payloads/:id", (req, res) => {
  const payload = deserializePayload(statements.get.get(req.params.id));
  if (!payload) return res.status(404).json({ error: "Payload not found" });
  res.json(payload);
});

app.post("/api/payloads", requireBearer, (req, res, next) => {
  try {
    const now = new Date().toISOString();
    const payload = {
      id: crypto.randomUUID(),
      topic: typeof req.body.topic === "string" ? req.body.topic : "r4/api",
      payload: req.body.payload ?? req.body,
      createdAt: now,
      updatedAt: now,
    };
    statements.insert.run(payload.id, payload.topic, JSON.stringify(payload.payload), now, now);
    res.locals.activityDetail = { payload };
    res.status(201).json(payload);
  } catch (error) {
    next(error);
  }
});

app.patch("/api/payloads/:id", requireBearer, (req, res, next) => {
  try {
    const existing = deserializePayload(statements.get.get(req.params.id));
    if (!existing) return res.status(404).json({ error: "Payload not found" });
    const payload = {
      ...existing,
      ...(typeof req.body.topic === "string" ? { topic: req.body.topic } : {}),
      ...(Object.hasOwn(req.body, "payload") ? { payload: req.body.payload } : {}),
      updatedAt: new Date().toISOString(),
    };
    statements.update.run(payload.topic, JSON.stringify(payload.payload), payload.updatedAt, payload.id);
    res.locals.activityDetail = { payload };
    res.json(payload);
  } catch (error) {
    next(error);
  }
});

app.delete("/api/payloads/:id", requireBearer, (req, res, next) => {
  try {
    const removed = deserializePayload(statements.get.get(req.params.id));
    if (!removed) return res.status(404).json({ error: "Payload not found" });
    statements.delete.run(req.params.id);
    res.locals.activityDetail = { payload: removed };
    res.status(204).end();
  } catch (error) {
    next(error);
  }
});

app.get("/api/activity", (_req, res) => res.json(activity));

app.use(express.static(publicDir));
app.get(/.*/, (_req, res) => res.sendFile(path.join(publicDir, "index.html")));

app.use((error, req, res, _next) => {
  const status = error.type === "entity.parse.failed" ? 400 : 500;
  record("api", "error", { ...requestDetails(req, status), message: error.message });
  res.status(status).json({ error: error.message });
});

await migrateLegacyPayloads();
app.listen(port, () => record("server", "started", { port }));
