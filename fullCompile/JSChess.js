const { Builder, By, until } = require('selenium-webdriver');
const fs = require('fs');
const net = require('net');
const robot = require('robotjs');
const firefox = require('selenium-webdriver/firefox');
const { empty } = require('cherio/lib/api/manipulation');
const { text } = require('cherio/lib/static');
const { end } = require('cherio/lib/api/traversing');

const BOARD_TOP_COORD = 184;
const BOARD_LEFT_COORD = 585;

function getTurnFromFEN(fen) {
    let spaceIndex = fen.indexOf(' ');
    if (spaceIndex !== -1) {
        let activeColor = fen.charAt(spaceIndex + 1);
        if (activeColor === 'w') {
            return 'w';
        } else if (activeColor === 'b') {
            return 'b';
        } else {
            return '0'; // Indicates an error: unknown active color
        }
    } else {
        return '0'; // Indicates an error: space not found
    }
}
async function tag(driver) {
    try {
        // Log the current action
        console.log("Waiting for the title element...");

        // Wait until the title element is present (which should be immediately after the page loads)
        await driver.wait(until.elementLocated(By.css('title')), 10000);

        // Log that the title element is located
        console.log("Title element located.");

        // Get the title of the page using JavaScript execution
        let title = await driver.executeScript('return document.title');
        
        // Log the title
        console.log("Title found: ", title);

        // If the title is not found or is empty, throw an error
        if (!title) {
            throw new Error('Title not found');
        }
        let titleWordsArray = title.split(/\s+/);
        // Log the words array
        console.log("Title as words array: ", titleWordsArray);

        // Return the title string
        return titleWordsArray[0].trim();
        
       
    }
    catch(error){
        
    }
}
async function captureScreenshot(driver, url) {
    await driver.get(url);
   
    await driver.wait(until.elementLocated(By.xpath('/html/body/div[2]/main/div[1]/div[1]/div/cg-container/cg-board')), 10000);

    let element = await driver.findElement(By.xpath('/html/body/div[2]/main/div[1]/div[1]/div/cg-container/cg-board'));
    let screenshot = await element.takeScreenshot();
    fs.writeFileSync('chessboard1.png', screenshot, 'base64');
    console.log('Screenshot captured and saved as chessboard1.png');

    let dimensions = await element.getRect();
    return {
        width: dimensions.width,
        height: dimensions.height
    };
}

async function getFENPart(driver, url) {
    let srccurl = 'view-source:' + url;
    await driver.get(srccurl);

    let pageSource = await driver.getPageSource();
    const regex = /"fen":"([^"]*)"/i;
    const match = pageSource.match(regex);

    if (match && match.length >= 1) {
        return match[1].trim();
    } else {
        console.log("No 'fen:' found in the HTML source.");
        return "";
    }
}

async function getMoves(driver, url) {
    await driver.get(url);
    await driver.wait(until.elementLocated(By.css("#main-wrap > main > div.round__app.variant-standard")), 10000);

    return await driver.executeScript(() => {
        let moveElements = document.querySelectorAll('kwdb'); // Updated selector
        return Array.from(moveElements).map(element => element.innerText.trim());
    });
}

async function sendMovesToServer(jsonData, retryCount = 5) {
    return new Promise((resolve, reject) => {
        const client = new net.Socket();

        const tryConnect = (attemptsLeft) => {
            if (attemptsLeft === 0) {
                return reject(new Error('Max retry attempts reached'));
            }

            client.connect(1337, '127.0.0.1', () => {
                client.write(jsonData);
            });

            client.on('data', (data) => {
                resolve(data.toString());
                client.destroy(); // kill client after server's response
            });

            client.on('error', (err) => {
                console.error(`Connection error: ${err.message}. Retrying in 5 seconds...`);
                client.destroy();
                setTimeout(() => tryConnect(attemptsLeft - 1), 5000);
            });
        };

        tryConnect(retryCount);
    });
}

async function overlayurl(driver) {
    await driver.get('https://lichess.org/@/abheekp/all');
  
    await driver.wait(until.elementLocated(By.css('#main-wrap > main > div > div.angle-content > div.search__result > div > article:nth-child(1) > a')), 10000);
    let element = await driver.findElement(By.css('#main-wrap > main > div > div.angle-content > div.search__result > div > article:nth-child(1) > a'));
    let url = await element.getAttribute('href');

    let pelement = await driver.findElement(By.css(`#main-wrap > main > div > div.angle-content > div.search__result > div > article:nth-child(1) > div.game-row__infos > div.versus > div.player.white`));

    let pNameRating = await pelement.getText();
    let pName = pNameRating.substr(0, 8).trim();
    console.log(pName);
    console.log(url);
    return { url, pName };
}
async function getUsercolor(driver){

    let userColor = document.querySelector("#main-wrap > main > div.round__app.variant-standard > div.round__app__board.main-board > div");

    if (userColor) {
      let className = userColor.className; // Retrieve the class name(s)
      console.log("User is: ",className);
      console.log("User is: ",className[1].trim());
      return className;
    } else {
      console.error('Could not find the specified element.');
      return null; // or handle the absence of the element as needed
    }

    
}

function delay(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

(async function main(arg1, arg2) {
    if(!arg2){
        arg2="Anon.";
    }

 
    let userName = arg2.trim();

    console.log(arg1);
    console.log(arg2);
 

    let ready = false;
    let options = new firefox.Options();
    options.addArguments('-headless'); // Set Firefox to run in headless mode

    let driver = await new Builder()
        .forBrowser('firefox')
        .setFirefoxOptions(options)
        .build();

    try {
   
        if(arg1.trim()){
            url = arg1;
            console.log(url);
        }
      
   // let f = await getUsercolor();
    
        await driver.get(url);
        let pName = await tag(driver); //White User
        
        let color = (pName == userName) ? "w" : "b";

        console.log('Player Name:', pName);
        //console.log("White User", whiteUsr);
        console.log('Player Color:', color);
    
        let previousMoves = [];

        while (true) {
            try {
                let moves = await getMoves(driver, url);
                console.log('Retrieved moves:', moves);

                let fenPart = await getFENPart(driver, url);
                let moveTurn = getTurnFromFEN(fenPart);
                console.log('FEN Part:', fenPart);
                console.log('Move Turn:', moveTurn);

                let newMoves = moves.slice(previousMoves.length);
                if (moveTurn == color && (newMoves.length>=1)) {
                    ready = true;
                }
                else{
                    ready = false;
                }

                if (ready === true) { 
                    console.log('It\'s your turn.');

                    const dimensions = await captureScreenshot(driver, url);

                    const jsonData = JSON.stringify({ color, dimensions, moves: newMoves, fenPart, ready });

                    console.log('New Moves:', newMoves.join(', '));
                    console.log('Sending JSON data:', jsonData);

                    try {
                        const response = await sendMovesToServer(jsonData);
                        console.log('Received data from C++ server:', response);

                        try {
                            const responseData = JSON.parse(response);
                            const bestMoves = responseData.bestMove;
                            console.log('Best Moves:', bestMoves);

                            if (bestMoves.length >= 3) {
                                let startChar = bestMoves.substr(0, 1);
                                let startNum = bestMoves.substr(1, 1);
                                let endChar = bestMoves.substr(2, 1);
                                let endNum = bestMoves.substr(3, 1);

                                console.log('Start Char:', startChar);
                                console.log('Start Num:', startNum);
                                console.log('End Char:', endChar);
                                console.log('End Num:', endNum);

                                const rowinc = 776 / 8;
                                const colinc = 776 / 8;
                                let grid = {};

                                if (color == "w" ) {
                                    for (let i = 0; i < 8; i++) {
                                        grid[8 - i] = [];
                                        for (let j = 0; j < 8; j++) {
                                            let x = j * colinc + BOARD_LEFT_COORD;
                                            let y = (BOARD_TOP_COORD + (i * rowinc));
                                            grid[8 - i][String.fromCharCode(97 + j)] = { x, y };
                                        }
                                    }
                                }  if (color === "b") {
                                    for (let i = 0; i < 8; i++) {
                                        grid[i + 1] = [];
                                        for (let j = 0; j < 8; j++) {
                                            let x = j * colinc + BOARD_LEFT_COORD;
                                            let y = (BOARD_TOP_COORD + (i * rowinc));
                                            grid[i + 1][String.fromCharCode(104 - j)] = { x, y };
                                        }
                                    }
                                }
                                
                                let startCoord = grid[startNum][startChar];
                                let endCoord = grid[endNum][endChar];
                                previousMoves = moves;

                                robot.moveMouse(startCoord.x, startCoord.y);
                                robot.mouseToggle('down');
                                robot.moveMouse(endCoord.x, endCoord.y);
                                robot.mouseToggle('up');

                                const performMove = async () => {
                                    await delay(2000);
                                      let moves = await getMoves(driver, url);
                                    let newMoves = moves.slice(previousMoves.length);

                                    if (newMoves.length < 1) { 
                                        robot.moveMouse(startCoord.x, startCoord.y);
                                        robot.mouseToggle('down');
                                        robot.moveMouse(endCoord.x, endCoord.y);
                                        robot.mouseToggle('up');

                                    }
                                   
                                };
                                   
                                    // Wait to ensure the move is completed
    
                                  
                              
                                await performMove();  
                                ready = false;
                               
                            } else {
                                console.error('Invalid bestMove format:', bestMoves);
                            }
                        } catch (error) {
                            console.error('Failed to parse response from server:', error);
                        }
                    } catch (error) {
                        console.error('Error sending moves to server:', error);
                    }
                } else {
                    console.log('Waiting for opponent move...');
                }

                await delay(2000); // Wait for 2 seconds before checking again

            } catch (error) {
                console.error('Error in main loop:', error);
            }
        }
    } catch (error) {
        console.error('Error in main try-catch:', error);
    } finally {
        if (driver) {
            try {
                await driver.quit();
            } catch (quitError) {
                console.error('Error quitting WebDriver:', quitError);
            }
        }
    }
})(process.argv[2], process.argv[3]);
