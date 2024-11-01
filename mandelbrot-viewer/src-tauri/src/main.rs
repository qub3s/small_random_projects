#![cfg_attr(
    all(not(debug_assertions), target_os = "windows"),
    windows_subsystem = "windows"
)]

use std::process::Command;
use std::io::{self, Write};
use std::str;

static mut DIST: f64 = 3.0;
static mut ZOOM: f64 = 0.66;
static mut X: f64 = -1.0;
static mut Y: f64 = 0.0;

#[tauri::command]
fn reset(){
    unsafe{
        DIST = 3.0;
        ZOOM = 0.66;
        X = -1.0;
        Y = 0.0;
        newimage(X,Y,DIST);
    }
}

#[tauri::command]
fn getdist() -> f64{
    unsafe{
        return DIST;
    }
}

#[tauri::command]
fn getzoom() -> f64{
    unsafe{
        return ZOOM;
    }
}

#[tauri::command]
fn getx() -> f64{
    unsafe{
        return X;
    }
}

#[tauri::command]
fn gety() -> f64{
    unsafe{
        return Y;
    }
}

#[tauri::command]
fn setdist(x: f64){
    unsafe{
        DIST = x;
    }
}

#[tauri::command]
fn setzoom(x: f64){
    unsafe{
        ZOOM = x;
    }
}

#[tauri::command]
fn setx(x: f64){
    unsafe{
        X = x;
    }
}

#[tauri::command]
fn sety(x: f64){
    unsafe{
        Y = x;
    }
}

#[tauri::command]
fn newimage(x: f64, y: f64, width: f64 ){
    let foutput = Command::new("pwd")
        .output()
        .expect("failed to execute pwd");
    let tmp = format!("{}{}",str::from_utf8(&foutput.stdout).unwrap().trim(), "/src/mandelbrot");
    println!("{}",tmp);
    
    let output = Command::new(tmp)
        .arg(x.to_string()).arg(y.to_string()).arg("1000").arg(width.to_string()).arg("16").arg("100").arg("../src/assets/test.bmp")
        .output()
        .expect("failed to execute process");

    io::stdout().write_all(&output.stdout).unwrap();
    io::stderr().write_all(&output.stderr).unwrap();
}

fn main() {
    unsafe{
        newimage(X,Y,DIST);
    }

    tauri::Builder::default()
        .invoke_handler(tauri::generate_handler![newimage,getdist,setdist,getzoom,setzoom,getx,setx,gety,sety,reset])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");

}
