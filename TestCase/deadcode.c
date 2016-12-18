/*
 * deadcode.c
 * Copyright (C) 2016 wang <wang@Yet-Another-MBP>
 *
 * Distributed under terms of the MIT license.
 */

int i = 1;

for(i=5;0;) {
    i = i + 1;
    println(i);
    if (i > 3) break;
}
