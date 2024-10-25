Introduction
============

Welcome to the official documentation of the Rune Engine, the free and open
source 2D and 3D game engine. This engine, being licensed under the terms of the
very liberal zlib license, allows game and application developers to create any
kind of game, for any platform and with no usage restriction whatsoever.
However, as the engine's focus is on Quake-style shooters, it may be more
difficult to create a different type of game (e.g., a top-down city builder) in
this engine. Developers are free to make their own modifications to Rune to
support their game and are encouraged to submit their changes to the mailing
lists.

This page gives a broad overview of the engine and this documentation, so that
you know where to start if you are a beginner or where to look if you need more
information about a particular feature.

Before you start
----------------

Please note that much of the engine is still under heavy development, and some
APIs have yet to be stabilized and documented. The ones listed in this
documentation have been stabilized and nothing should be removed or changed,
however, some functions may be added in the future should the need arise.

About the Rune Engine
---------------------

A standard, "off the shelf" game engine is designed to provide a developer with
an IDE-like interface with an editor, a publication platform, scripting engine,
etc. Rune is designed to be just another library that a developer can use. There
is no editor outside of the one you provide yourself. The engine is designed to
stay out of the developer's way as much as possible, allowing the developer to
realize their vision without distractions and fluff. Rune is built on POSIX and
Unix principles and abstractions, so Linux and macOS developers should feel very
at home working with Rune. The engine considers Linux a first-class citizen, so
any code that calls engine APIs and primitives should run *unmodified* on a
Linux system, given the application developer is only using Rune APIs.

Rune is completely free and open source under the permissive zlib license. No
strings attached and no royalities required. Users' games are theirs, down to
the last line of engine code. Rune's development is fully community-driven,
giving users the ability to shape the engine to meet their needs.
