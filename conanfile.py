"""Conan recipe for the Compressor."""

from conan import ConanFile
from conan.tools.cmake import cmake_layout


class CompressorRecipe(ConanFile):
    """Conan recipe for the Compressor project."""

    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self) -> None:
        """Declare the dependencies for the project."""
        self.requires("boost/1.82.0")
        self.requires("gtest/1.13.0")

    def build_requirements(self) -> None:
        """Declare the build requirements for the project.

        These are tools needed to build the project but aren't needed to use the resulting package.
        """
        self.build_requires("cmake/3.26.4")

    def layout(self) -> None:
        """Specify the layout of the package."""
        cmake_layout(self)

    def validate(self) -> None:
        """Validate the recipe.

        Raises an error for non-supported configurations.
        """
