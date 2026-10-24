const elements = {
  health: document.querySelector("#health"),
  payloadCount: document.querySelector("#payloadCount"),
  apiCount: document.querySelector("#apiCount"),
  serverCount: document.querySelector("#serverCount"),
  activity: document.querySelector("#activity"),
  payloads: document.querySelector("#payloads"),
  refresh: document.querySelector("#refresh"),
  columnToggles: [...document.querySelectorAll("[data-column-toggle]")],
};

const columnPreferenceKey = "arduino-playground-visible-columns";

function escapeHtml(value) {
  return String(value).replace(/[&<>'"]/g, (character) => ({
    "&": "&amp;", "<": "&lt;", ">": "&gt;", "'": "&#39;", '"': "&quot;",
  })[character]);
}

function readColumnPreferences() {
  try {
    return JSON.parse(localStorage.getItem(columnPreferenceKey)) ?? {};
  } catch {
    return {};
  }
}

function applyColumnPreferences() {
  const preferences = readColumnPreferences();
  for (const toggle of elements.columnToggles) {
    const column = toggle.dataset.columnToggle;
    if (column in preferences) toggle.checked = preferences[column];
    document.querySelectorAll(`[data-column="${column}"]`).forEach((cell) => {
      cell.hidden = !toggle.checked;
    });
  }
}

function saveColumnPreferences() {
  const preferences = Object.fromEntries(
    elements.columnToggles.map((toggle) => [toggle.dataset.columnToggle, toggle.checked]),
  );
  localStorage.setItem(columnPreferenceKey, JSON.stringify(preferences));
  applyColumnPreferences();
}

async function refresh() {
  try {
    const dashboardRequest = { headers: { "X-Monitor-Request": "dashboard" } };
    const [healthResponse, payloadResponse, activityResponse] = await Promise.all([
      fetch("/api/health", dashboardRequest),
      fetch("/api/payloads", dashboardRequest),
      fetch("/api/activity", dashboardRequest),
    ]);
    if (![healthResponse, payloadResponse, activityResponse].every((response) => response.ok)) {
      throw new Error("Serveren returnerede en fejl");
    }
    const [health, payloads, activity] = await Promise.all([
      healthResponse.json(), payloadResponse.json(), activityResponse.json(),
    ]);
    elements.health.textContent = "API online";
    elements.health.className = "status online";
    elements.payloadCount.textContent = payloads.length;
    elements.apiCount.textContent = activity.filter((event) => event.source === "api").length;
    elements.serverCount.textContent = activity.filter((event) => event.source === "server").length;
    elements.payloads.textContent = JSON.stringify(payloads, null, 2);
    const requests = activity.filter((event) => event.source === "api");
    elements.activity.innerHTML = requests.slice(0, 30).map((event) => {
      const detail = event.detail;
      return `
        <tr>
          <td class="time">${new Date(event.timestamp).toLocaleTimeString("en-GB")}</td>
          <td data-column="deviceId">${escapeHtml(detail.deviceId ?? "—")}</td>
          <td data-column="clientName">${escapeHtml(detail.clientName ?? "—")}</td>
          <td data-column="path"><code>${escapeHtml(detail.path ?? "—")}</code></td>
          <td data-column="method"><span class="method">${escapeHtml(detail.method ?? "—")}</span></td>
          <td data-column="status"><span class="http-status status-${Math.floor((detail.status ?? 0) / 100)}xx">${escapeHtml(detail.status ?? "—")}</span></td>
        </tr>`;
    }).join("") || '<tr><td class="empty" colspan="6">No external API requests yet.</td></tr>';
    applyColumnPreferences();
  } catch (error) {
    elements.health.textContent = "Server offline";
    elements.health.className = "status";
  }
}

elements.refresh.addEventListener("click", refresh);
elements.columnToggles.forEach((toggle) => toggle.addEventListener("change", saveColumnPreferences));
applyColumnPreferences();
refresh();
setInterval(refresh, 3000);
