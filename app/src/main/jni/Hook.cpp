// RAMBO startup compatibility fix.
// Intentionally no hooks or patches are installed into libanogs.so.
// The former implementation blocked several caller threads indefinitely and
// modified anti-cheat code at hard-coded offsets, which could stall game startup.
