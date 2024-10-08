function draw_border(ctx){
    ctx.strokeStyle = "#000000";
    ctx.lineWidth = 10;	
	ctx.beginPath();
	ctx.moveTo(1, 1);
	ctx.lineTo(w, 1);
	ctx.lineTo(w, h);
	ctx.lineTo(1, h);
	ctx.closePath();	
	ctx.stroke();
}

function draw_ball(ctx, x, y){
	ctx.beginPath();
	ctx.lineWidth = 2;
	ctx.moveTo(x+ballsize, y);
	ctx.arc(x, y, 10, 0, 2 * Math.PI);
	ctx.fillStyle = "#00FFFF";	
	ctx.fill();
	ctx.stroke();
}

// x is the center and should always be between 50 and w - 50
function draw_paddle(ctx,x){
	ctx.fillStyle = "#000000";
	ctx.fillRect( x - 50, h - 25, 100, 10);
}

function cord_to_pix(x,y){
    return [~~x, ~~y];
}	

function sleep(ms) {
	return new Promise(resolve => setTimeout(resolve, ms));
}

function redraw(ctx,x,y,s){
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    ctx.font = "20px Arial";
    ctx.fillText(s, 10, 30); 
	draw_border(ctx);
	draw_ball(ctx,x,y);
    draw_paddle(ctx,padpos);
}

function check_for_collision(x,y,vx,vy,s,ballpos){
    if (x < ballsize  || x > w-ballsize){
        vx *= -1;
    }

    if (y < ballsize ){
        vy *= -1;
    }

    if (vy >= 0 && y > h-ballsize-bottomspace && x > ballpos - 50 && x < ballpos + 50){
        vy *= -1;
        s = s + 1;
    }
    else if( y > h-ballsize ){
        s = 0;
        vx = 0;
        vy = 0;
    }

    return [vx,vy,s];
}

function move(){
    document.addEventListener("keypress", function(e) {
    if ( e.key == "D" || e.key == "d") {
        if(padpos + 30 + 50 <= w+30 ){
        padpos += 30;
        console.log(padpos);
        }
    }});
    
    document.addEventListener("keypress", function(e) {
    if ( e.key == "A" || e.key == "a") {
        if(padpos - 30 - 50 >= -30 ){
        padpos -= 30;
        console.log(padpos);
        }
    }});
}

async function cycle(){
	let x = w/2;
	let y = h/5;

    // get a vector of len 3
	let vx = Math.random();
	let vy = Math.random();
	let v = Math.sqrt(vx*vx+vy*vy)/5;
	vx = vx/v;
	vy = vy/v;
	

	if (canvas.getContext) {
        const ctx = canvas.getContext("2d");
		console.log(1);
        redraw(ctx,x,y);
		let sc = 0;
		while(true){
            x = x + vx;
            y = y + vy;
            let cord = cord_to_pix(x,y);
            await sleep(10);
            redraw(ctx,cord[0],cord[1], sc);
            let tmp = check_for_collision(x,y,vx,vy,sc,padpos);
            vx = tmp[0];
            vy = tmp[1];
            sc = tmp[2];
        }
	}
}

const canvas = document.getElementById("game");
var w = canvas.width;
var h = canvas.height;
var bottomspace =30;
var ballsize = 10;
var padpos = w/2;
move();
cycle();

