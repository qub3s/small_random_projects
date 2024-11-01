const std = @import("std");
const ray = @cImport({
    @cInclude("raylib.h");
});

// basic definitions
const stdout = std.io.getStdOut().writer();
const parseFloat = std.fmt.parseFloat;
const sliceTo = std.mem.sliceTo;
const momentum_struct = struct { mass: f128, x_pos: f128, y_pos: f128, x_vel: f128, y_vel: f128 };

fn draw_state(momentum: *[]momentum_struct) !void {
    const deref = momentum.*;

    ray.BeginDrawing();
    const scale = 10;

    for (0..deref.len) |i| {
        ray.DrawCircle(@intFromFloat(deref[i].x_pos / scale), @intFromFloat(deref[i].y_pos / scale), 10, ray.BLUE);
    }

    defer ray.EndDrawing();
    ray.ClearBackground(ray.RAYWHITE);
    return;
}

fn square(comptime T: type, x: T) T {
    return x * x;
}

// G * M1 * M2 / (r^2)   - Annahme das G = 1
fn calc_next_step(momentum: *[]momentum_struct) !void {
    const deref = momentum.*;

    for (0..deref.len) |i| {
        for (0..deref.len) |j| {
            if (i != j) {
                var direction_x: f128 = (deref[i].x_pos - deref[j].x_pos) / 2;
                var direction_y: f128 = (deref[i].y_pos - deref[j].y_pos) / 2;

                //try stdout.print("{} - {} \n", .{ direction_x, direction_y });
                //try stdout.print("{} - {} \n", .{ deref[i].x_pos, deref[i].y_pos });
                //try stdout.print("{} - {} \n\n", .{ deref[i].x_vel, deref[i].y_vel });

                const strenght: f128 = (deref[j].mass * deref[i].mass) / (square(f128, direction_x) + square(f128, direction_y));
                var scale: f128 = @sqrt(square(f128, direction_x) + square(f128, direction_y));
                scale = @max(scale, 1); // should not get too small otherwise excaleration is too large

                if (direction_x != 0 and direction_y != 0) {
                    direction_x = direction_x / scale * strenght;
                    direction_y = direction_y / scale * strenght;

                    deref[i].x_vel -= direction_x / deref[i].mass;
                    deref[i].y_vel -= direction_y / deref[i].mass;
                }
            }
        }
    }

    // edit the positions
    for (0..deref.len) |i| {
        deref[i].x_pos += deref[i].x_vel;
        deref[i].y_pos += deref[i].y_vel;
    }
    return;
}

pub fn main() !void {
    const argv = std.os.argv[1..];

    // variable declaration
    var general_purpose_allocator = std.heap.GeneralPurposeAllocator(.{}){};
    defer std.debug.assert(general_purpose_allocator.deinit() == .ok);
    const gpa = general_purpose_allocator.allocator();

    if (argv.len % 5 != 0 or argv.len == 0) {
        try stdout.print("The number of arguments cannot be zero and has to be dividable by 5 !!!\n", .{});
        return;
    }

    var momentum = try gpa.alloc(momentum_struct, argv.len / 5);

    for (argv, 0..) |arg, i| {
        switch (i % 5) {
            0 => {
                momentum[i / 5].mass = try parseFloat(f128, sliceTo(arg, 0));
            },
            1 => {
                momentum[i / 5].x_pos = try parseFloat(f128, sliceTo(arg, 0));
            },
            2 => {
                momentum[i / 5].y_pos = try parseFloat(f128, sliceTo(arg, 0));
            },
            3 => {
                momentum[i / 5].x_vel = try parseFloat(f128, sliceTo(arg, 0));
            },
            4 => {
                momentum[i / 5].y_vel = try parseFloat(f128, sliceTo(arg, 0));
            },
            else => {},
        }
    }

    const screenWidth = 800;
    const screenHeight = 800;

    ray.SetTraceLogLevel(4);
    ray.InitWindow(screenWidth, screenHeight, "window");
    defer ray.CloseWindow();

    ray.SetTargetFPS(240);
    //var count: i32 = 0;

    while (!ray.WindowShouldClose()) {
        //try stdout.print("{d}\n", .{count});
        //@mincount += 1;
        try draw_state(&momentum);
        try calc_next_step(&momentum);
    }

    gpa.free(momentum);
}
