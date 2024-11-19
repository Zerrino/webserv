document.getElementById("getButton").addEventListener("click", () => sendRequest("GET"));
document.getElementById("postButton").addEventListener("click", () => sendRequest("POST"));
document.getElementById("deleteButton").addEventListener("click", () => sendRequest("DELETE"));

function sendRequest(method) {
    const url = "/api/test";
    fetch(url, {
        method: method
    })
    .then(response => response.text())
    .then(data => {
        document.getElementById("response").innerText = `Réponse du serveur : ${data}`;
    })
    .catch(error => {
        document.getElementById("response").innerText = `Erreur : ${error}`;
    });
}
