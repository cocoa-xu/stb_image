defmodule StbImageDecodeBoundsTest do
  use ExUnit.Case, async: true

  alias StbImage.Nif

  for filename <- ["test-rgba.png", "test.jpg", "test.hdr"], channels <- 1..4 do
    @filename filename
    @channels channels

    test "decodes #{@filename} to #{@channels} channels from files and binaries" do
      path = Path.join(__DIR__, @filename)
      original = StbImage.read_file!(path)
      {height, width, _} = original.shape
      bytes_per_channel = if original.type == {:f, 32}, do: 4, else: 1

      from_file = StbImage.read_file!(path, channels: @channels)
      from_binary = StbImage.read_binary!(File.read!(path), channels: @channels)

      assert from_file.shape == {height, width, @channels}
      assert byte_size(from_file.data) == height * width * @channels * bytes_per_channel
      assert from_binary == from_file
    end
  end

  test "adding alpha preserves the RGB pixels" do
    path = Path.join(__DIR__, "test.jpg")
    original = StbImage.read_file!(path)
    expected = for <<r, g, b <- original.data>>, into: <<>>, do: <<r, g, b, 255>>

    assert StbImage.read_file!(path, channels: 4).data == expected
    assert StbImage.read_binary!(File.read!(path), channels: 4).data == expected
  end

  test "removing alpha preserves the RGB pixels" do
    path = Path.join(__DIR__, "test-rgba.png")
    original = StbImage.read_file!(path)
    expected = for <<r, g, b, _ <- original.data>>, into: <<>>, do: <<r, g, b>>

    assert StbImage.read_file!(path, channels: 3).data == expected
    assert StbImage.read_binary!(File.read!(path), channels: 3).data == expected
  end

  test "invalid channel requests return errors" do
    path = Path.join(__DIR__, "test.png")
    binary = File.read!(path)

    for channels <- [-1, 5, 2_147_483_647] do
      assert {:error, _} = Nif.read_file(path, channels)
      assert {:error, _} = Nif.read_binary(binary, channels)
    end
  end

  test "malformed images with explicit channels return errors" do
    for channels <- 0..4 do
      assert {:error, _} = Nif.read_binary(<<>>, channels)
      assert {:error, _} = Nif.read_file(Path.join(__DIR__, "stb-issue-1608.jpg"), channels)
    end
  end
end
