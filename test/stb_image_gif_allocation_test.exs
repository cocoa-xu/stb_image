defmodule StbImageGifAllocationTest do
  use ExUnit.Case, async: true

  test "GIF decoding recovers after truncated input" do
    binary = File.read!(Path.join(__DIR__, "test.gif"))

    for size <- 0..12 do
      assert {:error, _} = StbImage.read_gif_binary(binary_part(binary, 0, size))
    end

    assert {:ok, frames, delays} = StbImage.read_gif_binary(binary)
    assert length(frames) == length(delays)

    for frame <- frames do
      {h, w, c} = frame.shape
      assert byte_size(frame.data) == h * w * c
    end
  end
end
