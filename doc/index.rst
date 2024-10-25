.. Rune Game Engine documentation master file, created by
   sphinx-quickstart on Wed Oct 16 23:36:04 2024.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Rune Game Engine documentation
==============================

Rune is a high-performance game engine written entirely in C and designed to
support Quake-style, "boomer" shooters. It is free and open source, and has both
2D and 3D capabilities. If you are new to this documentation, we recommend that
you read the `introduction page <introduction.html>`_ to get an overview of features and specs that
Rune has to offer. Please note that improvements to the documentation are
welcome; join the rune-engine mailing list at lists.gymli.org if you want to help
out.

User-oriented documentation
---------------------------

The following manuals are written for game or application developers who are
trying to get the engine to work for their own applications or libraries.

.. toctree::
   :maxdepth: 1

   Introduction <introduction.rst>
   List of features <feature-list.rst>
   Core API <core/index.rst>
   Network API <network/index.rst>
   Rendering API <render/index.rst>
   UI API <ui/index.rst>
   Sound API <sound/index.rst>
   Reporting issues <reporting-issues.rst>

Working with upstream
---------------------

The guides for interacting with the engine's development community and getting
your work upstream.

.. toctree::
   :maxdepth: 1

   Development process <dev-process.rst>
   Coding style <coding-style.rst>
   Submitting patches <submitting-patches.rst>
