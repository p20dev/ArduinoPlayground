const elements = {
  health: document.querySelector("#health"),
  payloadCount: document.querySelector("#payloadCount"),
  apiCount: document.querySelector("#apiCount"),
  serverCount: document.querySelector("#serverCount"),
  activity: document.querySelector("#activity"),
  payloads: document.querySelector("#payloads"),
  refresh: document.querySelector("#refresh"),
};

function escapeHtml(value) {
  return String(value).replace(/[&<>'"]/g, (character) => ({
    "&": "&amp;", "<": "&lt;", ">": "&gt;", "'": "&#39;", '"': "&quot;",
  })[character]);
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
    elements.activity.innerHTML = activity.slice(0, 30).map((event) => {
      const caller = event.detail.caller;
      const callerName = caller?.deviceId || caller?.clientName || caller?.ip || "ukendt";
      const detail = { ...event.detail };
      delete detail.caller;
      return `
      <div class="event">
        <span class="source">${escapeHtml(event.source)}</span>
        <span class="time">${new Date(event.timestamp).toLocaleTimeString("da-DK")}</span>
        <span class="caller">${escapeHtml(callerName)}</span>
        <span class="detail"><strong>${escapeHtml(event.action)}</strong> ${escapeHtml(JSON.stringify(detail))}</span>
      </div>`;
    }).join("") || "<p>Ingen eksterne API-hændelser endnu.</p>";
  } catch (error) {
    elements.health.textContent = "Server offline";
    elements.health.className = "status";
  }
}

elements.refresh.addEventListener("click", refresh);
refresh();
setInterval(refresh, 3000);
