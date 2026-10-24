import crypto from "node:crypto";
import fs from "node:fs/promises";
import path from "node:path";
import express from "express";

const port = 3000;
const dataFile = process.env.DATA_FILE ?? "/data/payloads.json";
const bearerToken = process.env.API_BEARER_TOKEN ?? "";
const publicDir = path.resolve("public");
const activity = [];
let payloads = [];

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

async function loadPayloads() {
  try {
    payloads = JSON.parse(await fs.readFile(dataFile, "utf8"));
  } catch (error) {
    if (error.code !== "ENOENT") throw error;
    await persistPayloads();
  }
}

async function persistPayloads() {
  await fs.mkdir(path.dirname(dataFile), { recursive: true });
  const temporaryFile = `${dataFile}.tmp`;
  await fs.writeFile(temporaryFile, JSON.stringify(payloads, null, 2));
  await fs.rename(temporaryFile, dataFile);
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
      record("api", "request", requestDetails(req, res.statusCode));
    });
  }
  next();
});

app.get("/api/health", (_req, res) => {
  res.json({ status: "ok", timestamp: new Date().toISOString() });
});

app.get("/api/payloads", (_req, res) => res.json(payloads));

app.get("/api/payloads/:id", (req, res) => {
  const payload = payloads.find((item) => item.id === req.params.id);
  if (!payload) return res.status(404).json({ error: "Payload not found" });
  res.json(payload);
});

app.post("/api/payloads", requireBearer, async (req, res, next) => {
  try {
    const now = new Date().toISOString();
    const payload = {
      id: crypto.randomUUID(),
      topic: typeof req.body.topic === "string" ? req.body.topic : "r4/api",
      payload: req.body.payload ?? req.body,
      createdAt: now,
      updatedAt: now,
    };
    payloads.push(payload);
    await persistPayloads();
    res.status(201).json(payload);
  } catch (error) {
    next(error);
  }
});

app.patch("/api/payloads/:id", requireBearer, async (req, res, next) => {
  try {
    const index = payloads.findIndex((item) => item.id === req.params.id);
    if (index === -1) return res.status(404).json({ error: "Payload not found" });
    payloads[index] = {
      ...payloads[index],
      ...(typeof req.body.topic === "string" ? { topic: req.body.topic } : {}),
      ...(Object.hasOwn(req.body, "payload") ? { payload: req.body.payload } : {}),
      updatedAt: new Date().toISOString(),
    };
    await persistPayloads();
    res.json(payloads[index]);
  } catch (error) {
    next(error);
  }
});

app.delete("/api/payloads/:id", requireBearer, async (req, res, next) => {
  try {
    const index = payloads.findIndex((item) => item.id === req.params.id);
    if (index === -1) return res.status(404).json({ error: "Payload not found" });
    const [removed] = payloads.splice(index, 1);
    await persistPayloads();
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

await loadPayloads();
app.listen(port, () => record("server", "started", { port }));
