function sleep(ms) {
  return new Promise(resolve => setTimeout(resolve, ms));
}

async function runScroll(){

    var timeout = 12000;

    while (true) {
        window.scroll(0, document.getElementById("slide2").offsetTop);
        await sleep(timeout);
        window.scroll(0, document.getElementById("slide3").offsetTop);
        await sleep(timeout);
        window.scroll(0, document.getElementById("slide4").offsetTop);
        await sleep(timeout);
        window.scroll(0, document.getElementById("slide1").offsetTop);
        await sleep(timeout);
    }
}