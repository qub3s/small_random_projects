const { invoke } = window.__TAURI__.tauri;

async function setvars(){
    let d = await invoke("getdist");
    document.getElementById("dist").innerHTML = d ;
    document.getElementById("currentzoom").innerHTML  = await invoke("getzoom");
    document.getElementById("centx").innerHTML = await invoke("getx");
    document.getElementById("centy").innerHTML = await invoke("gety");
    document.getElementById("currentzoom").value = await invoke("getzoom");
}

async function reload_image(){
    document.getElementById(i).src = document.getElementById(i).src + "?time" + new Date(); 
}

async function getval(){
    let d = await invoke("getdist");
    let z = await invoke("getzoom");
    let tx = Number(document.getElementById("xvalue").innerHTML);
    let ty = Number(document.getElementById("yvalue").innerHTML);

    await invoke('newimage', { x: tx, y: ty, width: d*z});

    await invoke("setdist", { x: d*z});
    await invoke("setx", {x: tx});
    await invoke("sety", {x: ty});
    setvars();
    reload_image();
    return;
}

async function getPos(e){
    let dist = await invoke("getdist");
    let xval = await invoke("getx");
    let yval = await invoke("gety");
    
    document.getElementById("xvalue").innerHTML = ((e.clientX/1000-0.5) * dist + xval);
    document.getElementById("yvalue").innerHTML = ((e.clientY/1000-0.5) * -dist + yval);
    getval();
}

async function changezoom(){
    let tx = document.getElementById("currentzoom").value;
    await invoke("setzoom", { x: Number(tx)});
}

async function reset(){
    await invoke("reset");
}


window.onload = function(){ setvars() };
