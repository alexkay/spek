# Spek release checklist

 * Bump the version number in configure.ac.
 * `make -C po update-po && tx push -s`.
 * `tx pull -a` and commit with correct `--author`s.
 * Review translations, fix things like missing mnemonics.
 * Translate what I can, Ich spreek un peu по-русски.
 * `tx pull -a`.
 * Update the list of available languages, 80% or so will do:
   * po/LINGUAS
   * src/spek-preferences-dialog.cc
   * dist/win/bundle.sh
   * dist/osx/bundle.sh
 * Update authors using `git log vX.Y.Z.. --pretty=format:"%an <%ae>" | sort -u`.
 * Sync the list of authors with the About dialogue.
 * Update the copyright year in the About dialogue along with all the .po files.
 * Update news in README.md.
 * Update the website and debian/control's description with the new features.
 * Update the manpage, don't forget the release date. Run `make man`.
 * Update sshots if there are user-visible changes.
 * Update INSTALL.md with the new version number and instructions.
 * Update copyright year of touched code.
 * Make sure `make distcheck` doesn't return errors.
 * Prepare binaries for OSX and WIN, see README files in dist/*.
 * If anything has been changed, commit and re-make the tarball.
 * Update web/index.html: bump version numbers, binary sizes and news.
 * Write the blog post, link from web/index.html, don't publish yet.
 * Upload the tarball + OSX and WIN binaries.
 * `git tag -a vX.Y.Z`.
 * Upload the website using `make upload`.
 * Publish the blog post.
 * Send an announcement to the LAA list.
 * Update the forum thread on what.
 * Update FreeBSD and Debian ports.
