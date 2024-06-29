const { app, BrowserWindow, ipcMain } = require('electron');
const path = require('path');
const { exec } = require('child_process');

let mainWindow;
let batchProcess; // Variable to store reference to the batch file process

function createWindow() {
  mainWindow = new BrowserWindow({
    width: 800,
    height: 600,
    webPreferences: {
      preload: path.join(__dirname, 'preload.js')
    }
  });

  mainWindow.loadFile('indexui.html'); // Load your HTML file
}

app.whenReady().then(() => {
  createWindow();

  app.on('activate', () => {
    if (BrowserWindow.getAllWindows().length === 0) {
      createWindow();
    }
  });

  // Example: Listen for stop command from renderer process
  ipcMain.on('stop', () => {
    console.log('Received stop command. Stopping the application...');
    
    // Stop the batch file process if running
    if (batchProcess && !batchProcess.killed) {
      batchProcess.kill(); // Kill the batch file process
      console.log('Batch file process terminated.');
  
    }


  });

  ipcMain.on('submit-url', (event, url, user) => {
    console.log(`Received URL: ${url}`);
    console.log(`Received User: ${user}`);

    // Execute the batch file with the URL and user as arguments using exec
    let batFilePath = path.join(__dirname, 'start-servers.bat');
    
console.log(batFilePath);
    // Execute the batch file with the URL and user as arguments using exec
    batchProcess = exec(`ChessBot\\start-servers.bat "${url}" "${user}"`, (error, stdout, stderr) => {
      if (error) {
        console.error(`Error starting servers: ${error.message}`);
        event.reply('servers-start-error', error.message);
        return;
      }
      if (stderr) {
        console.error(`Error starting servers: ${stderr}`);
        event.reply('servers-start-error', stderr);
        return;
      }
      console.log(`Servers started successfully: ${stdout}`);
      event.reply('servers-started', stdout);
    });
  });
});


app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') {
    if (batchProcess && !batchProcess.killed) {
      batchProcess.kill(); // Kill the batch file process
      console.log('Batch file process terminated.');
    }

    // Quit the Electron application
    app.quit();
  
  }
});

