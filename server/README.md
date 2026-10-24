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
GET /api/payloads
GET /api/payloads/:id
```

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

WiFi-oplysningerne ligger kun i den lokale `.env` til brug for den kommende
board-sketch. De sendes ikke til browseren eller API-klienter.
