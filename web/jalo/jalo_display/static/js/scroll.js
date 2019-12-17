function sleep(ms) {
  return new Promise(resolve => setTimeout(resolve, ms));
}

async function runScroll(){

    // while (true) {
        window.scroll(0, document.getElementById("slide1").offsetTop);
        await sleep(10000);
        window.scroll(0, document.getElementById("slide2").offsetTop);
        await sleep(15000);
        window.scroll(0, document.getElementById("slide3").offsetTop);
        await sleep(30000);
        window.scroll(0, document.getElementById("slide4").offsetTop);
        await sleep(15000);
        window.scroll(0, document.getElementById("slide5").offsetTop);
        await sleep(10000);
        window.scroll(0, document.getElementById("slide1").offsetTop);
        // window.location.reload(true);
    // }
}
