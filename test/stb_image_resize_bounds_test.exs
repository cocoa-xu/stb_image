defmodule StbImageResizeBoundsTest do
  use ExUnit.Case, async: true

  alias StbImage.Nif

  test "resize rejects dimensions whose products overflow signed int" do
    for {height, width} <- [{65_536, 65_536}, {1, 2_147_483_647}] do
      assert {:error, _} = Nif.resize(<<0, 0, 0, 255>>, 1, 1, 4, height, width, 1)
      assert {:error, _} = Nif.resize(<<0, 0, 0, 255>>, height, width, 4, 1, 1, 1)
    end
  end

  test "resize preserves constant images for every supported pixel layout and type" do
    for channels <- 1..4, type <- [:u8, :f32] do
      pixel = if type == :u8, do: <<128>>, else: <<0.5::float-native-size(32)>>
      data = :binary.copy(pixel, channels)
      image = StbImage.new(data, {1, 1, channels}, type: type)
      resized = StbImage.resize(image, 2, 3)

      assert resized.shape == {2, 3, channels}
      assert byte_size(resized.data) == 6 * byte_size(data)

      if type == :u8 do
        assert resized.data == :binary.copy(data, 6)
      else
        for <<value::float-native-size(32) <- resized.data>> do
          assert_in_delta value, 0.5, 0.00001
        end
      end
    end
  end

  test "resize rejects invalid dimensions, channels, types and buffer lengths" do
    for dimension <- [-1, 0] do
      assert {:error, _} = Nif.resize(<<0>>, dimension, 1, 1, 1, 1, 1)
      assert {:error, _} = Nif.resize(<<0>>, 1, dimension, 1, 1, 1, 1)
      assert {:error, _} = Nif.resize(<<0>>, 1, 1, 1, dimension, 1, 1)
      assert {:error, _} = Nif.resize(<<0>>, 1, 1, 1, 1, dimension, 1)
    end

    for channels <- [-1, 0, 5, 2_147_483_647] do
      assert {:error, _} = Nif.resize(<<0>>, 1, 1, channels, 1, 1, 1)
    end

    for bytes <- [-1, 0, 2, 2_147_483_647] do
      assert {:error, _} = Nif.resize(<<0>>, 1, 1, 1, 1, 1, bytes)
    end

    for data <- [<<>>, <<0, 0>>] do
      assert {:error, _} = Nif.resize(data, 1, 1, 1, 1, 1, 1)
    end

    assert {:error, _} = Nif.resize(<<0>>, 1, 1, 1, 1, 1, 4)
  end
end
