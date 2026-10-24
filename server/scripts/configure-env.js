import crypto from "node:crypto";
import fs from "node:fs/promises";
import path from "node:path";
import { fileURLToPath } from "node:url";

const serverDir = path.dirname(path.dirname(fileURLToPath(import.meta.url)));
const repositoryRoot = path.dirname(serverDir);
const secretsPath = path.join(
  repositoryRoot,
  "sketches",
  "arduino-uno-r4-wifi",
  "wifi_matrix_status",
  "arduino_secrets.h",
);

const secrets = await fs.readFile(secretsPath, "utf8");
const readDefine = (name) => {
  const match = secrets.match(new RegExp(`^#define\\s+${name}\\s+"((?:\\\\.|[^"])*)"`, "m"));
  if (!match) throw new Error(`${name} was not found in ${secretsPath}`);
  return JSON.parse(`"${match[1]}"`);
};

const env = [
  "HOST_PORT=8152",
  "PUBLIC_DOMAIN=http://localhost",
  `API_BEARER_TOKEN=${crypto.randomBytes(32).toString("base64url")}`,
  `WIFI_SSID=${JSON.stringify(readDefine("SECRET_SSID"))}`,
  `WIFI_PASSWORD=${JSON.stringify(readDefine("SECRET_PASS"))}`,
  "",
].join("\n");

await fs.writeFile(path.join(repositoryRoot, ".env"), env, { mode: 0o600 });
console.log("Created .env with HOST_PORT=8152, PUBLIC_DOMAIN=http://localhost and local secrets.");
