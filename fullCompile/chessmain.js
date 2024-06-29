const express = require('express');
const path = require('path');
const { exec } = require('child_process');

const app = express();
const PORT = 3000;
let batchProcess;

app.use(express.json());

app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, 'indexui.html'));
});

app.post('/submit-url', (req, res) => {
  const { url, user } = req.body;
  console.log(`Received URL: ${url}`);
  console.log(`Received User: ${user}`);

  // Execute the batch file with the URL and user as arguments using exec
  let batFilePath = path.join(__dirname, 'start-servers.bat');
  console.log(batFilePath);

  batchProcess = exec(`${batFilePath} "${url}" "${user}"`, (error, stdout, stderr) => {
    if (error) {
      console.error(`Error starting servers: ${error.message}`);
      return res.status(500).json({ error: error.message });
    }
    if (stderr) {
      console.error(`Error starting servers: ${stderr}`);
      return res.status(500).json({ error: stderr });
    }
    console.log(`Servers started successfully: ${stdout}`);
    res.status(200).json({ message: stdout });
  });
});

app.post('/stop', (req, res) => {
  console.log('Received stop command. Stopping the application...');

  if (batchProcess && !batchProcess.killed) {
    batchProcess.kill();
    console.log('Batch file process terminated.');
    return res.status(200).json({ message: 'Batch file process terminated.' });
  }
  res.status(400).json({ error: 'No batch file process running.' });
});

app.listen(PORT, () => {
  console.log(`Server is running on http://localhost:${PORT}`);
  openDefaultBrowser(`http://localhost:${PORT}`);
});

// Function to open the default browser
function openDefaultBrowser(url) {
  const start =
    process.platform == "darwin"
      ? "open"
      : process.platform == "win32"
      ? "start"
      : "xdg-open";
  require("child_process").exec(start + " " + url);
}
