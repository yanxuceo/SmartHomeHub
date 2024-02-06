const SERVER_URL = 'http://192.168.0.12:80/get_command';


async function fetchCommandFromServer() {
  try {
    const response = await fetch(SERVER_URL);
    const data = await response.json();
    return data.command;
  } catch (err) {
    console.error('Error fetching command:', err);
    return 'none';
  }
}

let lastExecutedCommand = null;

// Separate commands into state and action categories
const stateCommands = ['play', 'pause'];
const actionCommands = ['fastforward', 'backward', 'increaseVolume', 'decreaseVolume'];

async function executeCommandOnYouTubeTab(command) {
    if(command === lastExecutedCommand || command === 'none') {
        return;
    }

    // Check if YouTube tab is active
    const [tab] = await chrome.tabs.query({ url: '*://*.youtube.com/*', active: true, currentWindow: true });
  
    if (tab) {
        let funcToExecute;
        switch (command) {
        case 'play':
            funcToExecute = play;
            break;
        case 'pause':
            funcToExecute = pause;
            break;
        // Add other cases as needed
        case 'fastforward':
            funcToExecute = fastForward;
            break;
        case 'backward':
            funcToExecute = playback;
            break;

        case 'increaseVolume':
            funcToExecute = increaseVolume;
            break;

        case 'decreaseVolume':
            funcToExecute = decreaseVolume;
            break;

        default:
            return;
        }
        chrome.scripting.executeScript({
        target: { tabId: tab.id },
        func: funcToExecute
        });

        // Only update lastExecutedCommand for state commands
        if (stateCommands.includes(command)) {
            lastExecutedCommand = command;
        }
    }
}

function play() {
    const video = document.querySelector('video');
    if (video && video.paused) video.play();
}

function pause() {
    const video = document.querySelector('video');
    if (video && !video.paused) video.pause();
}

function fastForward() {
    const video = document.querySelector('video');
    if (video) {
        video.currentTime += 10; // fast forward by 10 seconds
    }
}

function playback() {
    const video = document.querySelector('video');
    if (video) {
        video.currentTime -= 10; // rewind by 10 seconds
    }
}

function increaseVolume() {
    const video = document.querySelector('video');
    if (video) {
        video.volume = Math.min(1, video.volume + 0.1); // Increase volume by 10%, but don't exceed 1.0
    }
}

function decreaseVolume() {
    const video = document.querySelector('video');
    if (video) {
        video.volume = Math.max(0, video.volume - 0.1); // Decrease volume by 10%, but don't go below 0
    }
}


// Poll server every 200ms
setInterval(async () => {
    const command = await fetchCommandFromServer();
    await executeCommandOnYouTubeTab(command);
}, 200);
