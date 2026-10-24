# Arduino Playground-server

Compose-stacken leverer:

- webdashboard på `/`;
- REST API på `/api`.

## Start

Opret den lokale `.env` fra WiFi-status-sketchens secrets og start stacken:

```powershell
npm --prefix server run configure
docker compose up -d --build
```

Med standardopsætningen åbnes dashboardet på `http://localhost:8152`.

## API

Læsning er offentlig:

```text
GET /api/health
GET /api/activity
GET /api/payloads?limit=100&offset=0
GET /api/payloads/count
GET /api/payloads/:id
```

Payloads gemmes i SQLite med WAL-mode. Liste-endpointet returnerer de nyeste
100 records som standard, understøtter `limit` (maks. 1000) og `offset`, og
sender det samlede antal i `X-Total-Count`-headeren. En eksisterende
`/data/payloads.json` migreres automatisk, når SQLite-databasen oprettes, og
fjernes efter en vellykket migrering.

Skrivning kræver `Authorization: Bearer <API_BEARER_TOKEN>`:

```text
POST   /api/payloads
PATCH  /api/payloads/:id
DELETE /api/payloads/:id
```

Eksempel på body til `POST`:

```json
{
  "topic": "r4/status",
  "payload": {
    "temperature": 22.4,
    "online": true
  }
}
```

Boardet bør identificere sig på alle kald. Aktivitetsdashboardet prioriterer
`X-Device-ID`, derefter `X-Client-Name` og til sidst klientens IP-adresse:

```text
X-Device-ID: uno-r4-wifi-01
X-Client-Name: wifi-status-sketch
```

Dashboardets egne forespørgsler markeres internt og gemmes ikke i
aktivitetsloggen. GET-kald uden `X-Device-ID` eller `X-Client-Name` udelades
også, så anonyme dashboard- og overvågningskald ikke overdøver boardtrafikken.

WiFi-oplysningerne ligger kun i den lokale `.env` til brug for den kommende
board-sketch. De sendes ikke til browseren eller API-klienter.
