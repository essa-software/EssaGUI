# EventLoop
    _______________
    |             |
    |  EventLoop  |
    |             |
    ---------------

## Introdution

Path: "EssaGUI\EventLoop.hpp"

Description: Event Loop is an abstract class for window updates in Essa Application.

## Methods

#### public virtual ~EventLoop() = default;

Basic virtual destructor

#### public void run();

Method used to start EssaGUI Event Loop.

#### public void quit();

Method used to end EssaGUI Event Loop.

#### public float tps() const;

Getter for EssaGUI window ticks per second.

#### public bool is_running() const;

Returns true if window is running.

#### private virtual void tick() = 0;

Abstract function which updates EssaGUI windows every tick.

## Properties

#### private bool m_running = true;

Is true when window is running

#### private float m_tps = 0;

stores TPS information
