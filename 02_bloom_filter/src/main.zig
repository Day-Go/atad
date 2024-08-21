const std = @import("std");

pub fn main() !void {
    var filter: u128 = 0;
    const filter_size: u7 = 127;
    const k: u2 = 3;

    const set = [_]u8{ 'a', 'b', 'c' };
    for (set) |element| {
        for (0..k) |i| {
            const hash: u64 = generate_hash(element, i);
            std.debug.print("Hash of '{c}': {b}\n", .{ element, hash });

            const index = @as(u7, @intCast(hash % filter_size));
            filter |= (@as(u128, 1) << index);
            std.debug.print("State of filter: {b}\n", .{filter});
        }
        std.debug.print("\n", .{});
    }

    const items_to_test = [_]u8{ 'a', 'd', 'b', 'e' };
    for (items_to_test) |item| {
        var is_member = true;
        for (0..k) |i| {
            const hash: u64 = generate_hash(item, i);
            std.debug.print("Hash of '{c}': {b}\n", .{ item, hash });
            const index = @as(u7, @intCast(hash % filter_size));
            is_member = is_member and ((filter & (@as(u128, 1) << index)) != 0);
        }
        std.debug.print("Is '{c}' a member? {}\n", .{ item, is_member });
    }
}

const generate_hash = struct {
    fn generate(element: u8, seed: usize) u64 {
        var hasher = std.hash.Wyhash.init(seed);
        std.hash.autoHash(&hasher, element);
        const hash = hasher.final();
        return hash;
    }
}.generate;
