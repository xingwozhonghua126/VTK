cmake_minimum_required(VERSION 3.12)

# Where Python for wheels are stored.
set(python_url_root "https://www.paraview.org/files/dependencies/python-for-wheels")

# Python version specifics.
set(python36_version "3.6.8")
set(python37_version "3.7.9")
set(python38_version "3.8.8")
set(python39_version "3.9.4")

# Hashes for various deployments.
set(python36_windows_x86_64_hash "")
set(python37_windows_x86_64_hash "")
set(python38_windows_x86_64_hash "")
set(python39_windows_x86_64_hash "")

set(python36_macos_x86_64_hash "bd3b68dfc9787c39312c8bd554853fe750abd999e100690c3ed81c29447b02d3")
set(python37_macos_x86_64_hash "1d31a228ac921c13787f74e0b9e7a04ae5806e70c5226e23711840bf0f0c9e90")
set(python38_macos_x86_64_hash "0665ce12462b0cc91afeec26355306136325ce49c8fa388a0e5f3ccf384d1e20")
set(python39_macos_x86_64_hash "24154d1571453bee30053b0103a9c47d48969014d9bda4a29174c66ae4efd46e")

# Extracting information from the build configuration.
if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "windows")
  set(python_platform "windows")
elseif ("$ENV{CMAKE_CONFIGURATION}" MATCHES "macos")
  set(python_platform "macos")
else ()
  message(FATAL_ERROR
    "Unknown platform for Python")
endif ()

if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "36_")
  set(python_version 36)
elseif ("$ENV{CMAKE_CONFIGURATION}" MATCHES "37_")
  set(python_version 37)
elseif ("$ENV{CMAKE_CONFIGURATION}" MATCHES "38_")
  set(python_version 38)
elseif ("$ENV{CMAKE_CONFIGURATION}" MATCHES "39_")
  set(python_version 39)
else ()
  message(FATAL_ERROR
    "Unknown version for Python")
endif ()

if ("$ENV{CMAKE_CONFIGURATION}" MATCHES "x86_64")
  set(python_arch "x86_64")
else ()
  message(FATAL_ERROR
    "Unknown architecture for Python")
endif ()

# Figure out what file we're supposed to download.
set(python_subdir "python-${python${python_version}_version}-${python_platform}-${python_arch}")
set(filename "${python_subdir}.tar.xz")
set(sha256sum "${python${python_version}_${python_platform}_${python_arch}_hash}")

# Verify that we have a hash to validate.
if (NOT sha256sum)
  message(FATAL_ERROR
    "Unsupported configuration ${python_platform}/${python_arch} ${python${python_version}_version}")
endif ()

# Download the file.
file(DOWNLOAD
  "${python_url_root}/${filename}"
  ".gitlab/${filename}"
  STATUS download_status
  EXPECTED_HASH "SHA256=${sha256sum}")

# Check the download status.
list(GET download_status 0 res)
if (res)
  list(GET download_status 1 err)
  message(FATAL_ERROR
    "Failed to download ${filename}: ${err}")
endif ()

# Extract the file.
execute_process(
  COMMAND
    "${CMAKE_COMMAND}"
    -E tar
    xf "${filename}"
  WORKING_DIRECTORY ".gitlab"
  RESULT_VARIABLE res
  ERROR_VARIABLE err
  ERROR_STRIP_TRAILING_WHITESPACE)
if (res)
  message(FATAL_ERROR
    "Failed to extract ${filename}: ${err}")
endif ()

# Move to a predictable prefix.
file(RENAME
  ".gitlab/${python_subdir}"
  ".gitlab/python")
