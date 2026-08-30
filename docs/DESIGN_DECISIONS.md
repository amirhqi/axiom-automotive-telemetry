# Design Decisions

## QML for presentation, C++ for application logic

The QML layer is responsible for presentation and interaction. Transport, parsing, logging and application
state remain in C++ so they are independently testable.

## Worker-thread I/O

Socket and Serial lifecycle work is kept away from the UI thread. This avoids coupling rendering responsiveness
to I/O timing and provides a realistic architecture for later hardware-facing development.

## Parser independent from transports

A parser should not need a socket to be testable. Transport-specific code produces application frames and the
parser validates them.

## Bounded history

Engineering tools may run for hours. History is bounded by design instead of relying on the user to restart
the application.

## MQTT, cloud, ML and 3D are deliberately deferred

The project first demonstrates reliable C++/Qt/Linux-oriented application fundamentals. Additional technology is
only useful when it serves a concrete system requirement.