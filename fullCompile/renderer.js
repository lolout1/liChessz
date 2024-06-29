

async function validateInput(url, user) {
  const urlPattern = /lichess\.org/;
  if (!urlPattern.test(url.trim())) {
    alert('Invalid URL. Please enter a URL that starts with "https://lichess.org" or "http://lichess.org".');
    return false;
  }

  if (!user || user=='') {
    alert('Username is be empty.');
    user="Anon."
    return true;
  }
  if(user){
    let exists = await checkUserExists(user.trim());
  if (!exists) {
    alert('Username does not exist. Please enter a valid username or leave blank');
    return false;
  }}

  return true;
}

async function checkUserExists(user) {
  try {
    const response = await fetch(`https://lichess.org/@/${user}`, {
      method: 'HEAD'
    });
    return response.ok;
  } catch (error) {
    alert('Error checking user existence:', error);
    return false;
    
  }
}
document.addEventListener('DOMContentLoaded', () => {
var { electronIPC } = window; // Access the exposed electronIPC object

var urlInput = document.getElementById('url');
var userName = document.getElementById('user');
var submitBtn = document.getElementById('submitBtn');
var quitBtn = document.getElementById('quitBtn');

submitBtn.addEventListener('click', async () => {
  var url = urlInput.value.trim();
  var user = userName.value.trim();
  console.log('Input value:', url); // Log the input value to verify

  if (url) {
    let isValid = await validateInput(url, user);
    if (isValid) {
      electronIPC.send('submit-url', url, user); // Send URL to main process via IPC
      urlInput.value = ''; // Clear input fields
      userName.value = '';
    } else {
      
      
    }
  }
 
});
quitBtn.addEventListener('click', () => {
  electronIPC.send('stop');
  window.location.reload(); // Send quit event to main process
});
// Example: Handle responses from main process
electronIPC.receive('servers-started', (message) => {
  console.log('Servers started successfully:', message);
  window.location.reload();
  // Handle success notification in your UI if needed
});

electronIPC.receive('servers-start-error', (errorMessage) => {
  console.error('Error starting servers:', errorMessage);
  alert('Error starting servers:', errorMessage);


  // Handle error notification in your UI if needed
});

});  