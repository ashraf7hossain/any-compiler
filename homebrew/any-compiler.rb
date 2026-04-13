class AnyCompiler < Formula
  desc "Compile code in any language via OneCompiler API"
  homepage "https://github.com/ashraf7hossain/any-compiler"
  license "MIT"
  
  on_macos do
    if Hardware::CPU.arm?
      url "https://github.com/ashraf7hossain/any-compiler/releases/download/v1.0.0/any-compiler-darwin-arm64"
      sha256 "DARWIN_ARM64_SHA256_HERE"
    else
      url "https://github.com/ashraf7hossain/any-compiler/releases/download/v1.0.0/any-compiler-darwin-amd64"
      sha256 "DARWIN_AMD64_SHA256_HERE"
    end
  end
  
  on_linux do
    if Hardware::CPU.arm? && !Hardware::CPU.is_64_bit?
      url "https://github.com/ashraf7hossain/any-compiler/releases/download/v1.0.0/any-compiler-linux-arm"
      sha256 "LINUX_ARM_SHA256_HERE"
    elsif Hardware::CPU.arm?
      url "https://github.com/ashraf7hossain/any-compiler/releases/download/v1.0.0/any-compiler-linux-arm64"
      sha256 "LINUX_ARM64_SHA256_HERE"
    else
      url "https://github.com/ashraf7hossain/any-compiler/releases/download/v1.0.0/any-compiler-linux-amd64"
      sha256 "LINUX_AMD64_SHA256_HERE"
    end
  end

  def install
    bin.install "any-compiler"
  end

  test do
    system "#{bin}/any-compiler", "--version"
  end
end
