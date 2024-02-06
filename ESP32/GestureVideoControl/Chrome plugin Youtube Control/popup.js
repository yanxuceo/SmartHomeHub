document.addEventListener('DOMContentLoaded', function() {
    document.getElementById('playPause').addEventListener('click', function() {
      chrome.tabs.query({active: true, currentWindow: true}, function(tabs) {
        chrome.scripting.executeScript({
          target: {tabId: tabs[0].id},
          function: playPause
        });
      });
    });
  
    document.getElementById('fastForward').addEventListener('click', function() {
      chrome.tabs.query({active: true, currentWindow: true}, function(tabs) {
        chrome.scripting.executeScript({
          target: {tabId: tabs[0].id},
          function: fastForward
        });
      });
    });
  });
  
  function playPause() {
    const video = document.querySelector('video');
    if(video) {
      if(video.paused) video.play();
      else video.pause();
    }
  }
  
  function fastForward() {
    const video = document.querySelector('video');
    if(video) {
      video.currentTime += 10;  // Skip forward by 10 seconds
    }
  }
  