# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.2.0] - 2021-04-16

### Added

- Add `default.platform` to allow any empty project to be converted as a Platform project (no longer need to import a MicroEJ Architecture first).
- Set default module version to `0.1.0` as other MicroEJ SDK skeletons.
- Set default module organization to `com.mycompany` as other MicroEJ SDK skeletons.
- Use a new private configuration ``embedded`` as the default for the platform dependencies.  This prevents an instance of "Too many loaded Platforms" error in `platform-loader`.

### Fixed

- Fix documentation in `bsp.properties`:

  - The variable `${project.prefix}` is no longer supported.
  - The variables ``*.relative.dir`` are relative to the BSP root directory.

- Fix execution of `run.[bat|sh]` in the directory where is defined `application.out`.
- Fix module configurations for correct Platform module import (especially in a module repository)
- Fix module build crash when no README file is declared (optional)

## [1.1.0] - 2021-01-08

### Changed

 - Added scripts and `module.ivy` file for Platform module build and publication.

## [1.0.0] - 2020-06-24

### Added

  - Initial version with scripts for BSP connection.
  
---
_Copyright 2020-2021 MicroEJ Corp. All rights reserved._
_This library is provided in source code for use, modification and test, subject to license terms._
_Any modification of the source code will break MicroEJ Corp. warranties on the whole library._
