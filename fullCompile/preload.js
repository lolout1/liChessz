// preload.js

const { contextBridge, ipcRenderer } = require('electron');

contextBridge.exposeInMainWorld('electronIPC', {
  send: (channel, data, data1) => {
    ipcRenderer.send(channel, data, data1);
  },
  receive: (channel, func) => {
    ipcRenderer.on(channel, (event, ...args) => func(...args));
  }
});
