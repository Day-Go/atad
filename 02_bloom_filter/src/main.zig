const std = @import("std");

pub fn main() !void {
    const set = [_]u8{ 'a', 'b', 'c' };

    for (set) |element| {
        var hasher = std.hash.Wyhash.init(0);
        std.hash.autoHash(&hasher, element);
        const hash = hasher.final();

        std.debug.print("Hash of '{c}': {b}\n", .{ element, hash });
    }
}
