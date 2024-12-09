/* EMS */
var Module = {
    canvas: (function() { return document.querySelector('#canvas'); })()
};

const games = [];

function gameStarted() {
    currentGame = {
        timeStarted: new Date(),
        score: null,
        timeEnded: null
    };
}

function gameQuit() {
    currentGame = null;
}

function gameEnded(score) {
    if (currentGame === null) {
        return;
    }
    currentGame.score = score;
    currentGame.timeEnded = new Date();

    games.push(currentGame);

    currentGame = null;

    prettyPrintStatus();
}

function getHiscore() {
    return games.reduce((max, game) => (game.score > max.score ? game : max), games[0]).score;
}

function formatDate(date) {
    const pad = (num) => String(num).padStart(2, '0');

    const day = pad(date.getDate());
    const month = pad(date.getMonth() + 1);
    const year = date.getFullYear();

    const hours = pad(date.getHours());
    const minutes = pad(date.getMinutes());
    const seconds = pad(date.getSeconds());

    return `${day}.${month}.${year} ${hours}:${minutes}:${seconds}`;
}

function calculateDuration(startTime, endTime) {
    const durationMs = endTime - startTime;
    const totalSeconds = Math.floor(durationMs / 1000);
    const minutes = Math.floor(totalSeconds / 60);
    const seconds = totalSeconds % 60;

    return `${String(minutes).padStart(2, '0')}:${String(seconds).padStart(2, '0')}`;
}

function prettyPrintStatus() {
    var lastGame = games[games.length-1];

    document.querySelector("#stats").style.display = "block";

    document.querySelector("#hiscore").innerText = getHiscore();
    document.querySelector("#lastgame").innerText = `Game ${games.length} - ${calculateDuration(lastGame.timeStarted, lastGame.timeEnded)} - Score: ${lastGame.score}`;
}