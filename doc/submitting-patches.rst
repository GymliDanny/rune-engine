Submitting patches
==================

If you have experience submitting patches to the Linux kernel or similar
projects, you are already familiar with the system Rune uses for
community-driven development. If not, this is the guide from which you should
begin. This manual provides a step by step guide on how to get your code into
the engine, as well as tips on increasing the odds your changes get merged into
the mainline release.

Experience with ``git`` is expected and assumed, as the engine uses it for all
it version control needs. If you are unfamiliar with it, the documentation for
``git`` can be found at `this website <https://git-scm.com/doc>`_.

Obtain a source tree
----------------------------

If you do not have a repository with the engine source, use ``git`` to download
one. Most of the time, you will want a mainline repository, which can be cloned
like so:

.. code-block::

   git clone git://git.gymli.org/dholman/rune-engine.git

Describe your changes
---------------------

For any feature, bugfix or solution, there must have been a motivation behind
this change. Convince the reviewer that there is a problem worth fixing or a new
feature to be added, and make them believe its worth reading past the first few
lines of the commit message.

Optimizations and the trade-offs that were made with them should be made clear
in the message. If you claim a performance increase, back it up with a numerical
figure. Describe the expected downsides of your optimization so that the
reviewer and the community at large can discuss its potential benefits and
costs.

Patches should fix only a single problem or introduce a single feature. If the
changes you send do more than that, consider breaking them up into a series of
patches.

When a patch is submitted, the full description and justification should be
included with it. It is on you to ensure the correct information is attached to
the patch or patch series. Maintainers are busy guys and are not obligated to
follow a URL to a pastebin to find the information about a patch. In other
words, the patch (series) should be self-contained.

If you need to link to a mailing list archive, prefer to use the lists.gymli.org
service. To create the link, use the contents of the ``Message-ID`` header of
the message without the angle brackets.

.. code-block::

   Link: https://lists.gymli.org/rune-engine/104

Your changes should be in imperative mood and should follow the canonical patch
format. In other words, the patch should sound as though it is ordering the code
to change, rather than the developer merely describing his or her changes. This
makes commit messages shorter and easier to follow. Furthermore, the message
should be in plain English or a very close approximation as can be permitted
when discussing technical details.

If you want to refer to a specific commit or issue, don't just include the hash
of the commit or the issue ID. The patch should contain at least a oneline
summary of the commit or issue. This makes code review flow much more smoothly
and allows the maintainers to know what you are referring to. The hash should at
least include the first twelve characters of the SHA-1 hash. This reduces the
chance of collisions even when the number of objects grows large.

If a patch fixes a bug introduced in a previous commit or a bug tracker, the
'Fixes:' tag should be used with the first twelve characters of the SHA-1 hash,
or in the case of a public bug tracker, the 'Closes:' tag should be used with
the relevant link.

.. code-block::
   
   Closes: https://bugs.site.tld/issues/176
   Fixes: 123456789abc ("subsystem: file: example")

Select the recipients for your patch
------------------------------------

You should always CC the appropriate maintainers and lists on any patch to code
they maintain. A good bet is to send it to the email attached to the
``git blame`` data and the main discussion list. If you cannot contact the
relevant maintainer for the subsystem you are working on, Danny Holman, being
the final arbiter of all changes accepted into the engine, serves as maintainer
of last resort.

Formatting
----------

Danny and other engine developers need to be able to read and comment on the
changes you are submitting. If a maintainer cannot quote your changes using
standard, plain text tools, it is almost guaranteed to be ignored. For this
reason, all patches should be submitted by email "inline". By far, the easiest
way to do this is to use ``git send-email``. The documentation linked in the
opening paragraphs has a section dedicated to this tool.

Furthermore, all patches should contain 'PATCH' in the subject line. This makes
actual patches distinguishable from other email discussions. The
``git send-mail`` tool will do this automatically.

Sign your work
--------------

To facilitate copyright and to keep track of who submitted changes, all patches
should include a sign-off at the end of the commit message. This certifies you
wrote the patch or otherwise have the right to pass it on as open source. See
the ``dco.txt`` file for the exact requirements.

When committing, ``git`` will do this automatically when you specify
``--signoff`` as part of the command flags. Alternatively, you can define an
alias in your ``.gitconfig`` file that will shorten ``git commit --signoff`` to
something similar to ``git cs``.

Canonical patch format
----------------------

The canonical patch subject line is:

.. code-block::

   Subject: [PATCH 1/X] subsystem: summary phrase

The body contains the following:

* A ``from`` line specifying the patch author, followed by an empty line
* The body of the explanation, line wrapped at 75 columns, which will be copied
  to the permanent changelog to describe this patch
* An empty line
* The ``Signed-off-by:`` lines, described above
* A marker line containing simply ``---``
* Any additional comments not suitable for the changelog
* The actual patch (``diff`` output)

References
----------

* `Linux kernel patch submission guide <https://www.kernel.org/doc/html/latest/process/submitting-patches.html>`_
