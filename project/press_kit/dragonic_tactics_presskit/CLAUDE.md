# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is the press kit repository for **Dragonic Tactics**, a D&D-style turn-based tactical RPG. The `reference/` directory contains a complete, saved copy of the Young Horses (Octodad: Dadliest Catch) press kit as a structural template. It was generated with the [presskit() tool](http://dopresskit.com/) by Rami Ismail (Vlambeer).

The actual Dragonic Tactics press kit pages should follow this same structure and conventions.

## Previewing

No build step required. Open any `.html` file directly in a browser.

## Press Kit Page Structure

Each press kit page is a single HTML file with a `<page-name>_files/` sibling directory for assets. Every page follows this section order:

1. **Factsheet** — developer name/location, release dates by platform, platform links, website, price table
2. **Description** — short game summary
3. **History** — development backstory
4. **Features** — bullet list
5. **Videos** — embedded YouTube iframes (each video is a separate `.html` file in `_files/`)
6. **Images** — screenshot gallery using Masonry grid layout
7. **Logo & Icon** — branding assets with zip download link
8. **Selected Articles** — press quotes with source citations
9. **Monetization Permission** — explicit statement allowing video monetization
10. **Additional Links** — DLC, related content
11. **About** — developer boilerplate
12. **Team / Credits** — name + role per person
13. **Contact** — business inquiries, press requests, web

## Layout & Tech

- **2-column layout**: left 1/4 nav sidebar (`#navigation`), right 3/4 content (`#content`)
- **CSS framework**: UIKit (`uikit.gradient.min.css`) + custom overrides in `style.css`
- **Image grids**: jQuery + Masonry (`masonry.pkgd.min.js`) with `imagesLoaded` plugin
- **Navigation**: anchor links (`#factsheet`, `#description`, etc.) to sections within the page

## Asset Conventions

- Supporting assets (images, CSS, JS, iframe HTMLs) live in `<page-name>_files/` next to the HTML file.
- The reference JS files have a `.다운로드` extension (Korean browser download artifact from the saved page) — strip this suffix in production files.
- YouTube embeds are saved as individual iframe HTML files inside `_files/`, one per video.
