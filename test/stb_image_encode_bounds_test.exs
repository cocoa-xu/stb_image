defmodule StbImageEncodeBoundsTest do
  use ExUnit.Case, async: true

  alias StbImage.Nif

  @tag :tmp_dir
  test "encoders reject malformed buffers before creating files", %{tmp_dir: tmp_dir} do
    for format <- [:png, :bmp, :tga, :jpg, :hdr],
        {data, height, width, channels} <- [
          {<<>>, 1, 1, 1},
          {<<0, 0>>, 1, 1, 1},
          {<<0>>, 0, 1, 1},
          {<<0>>, 1, -1, 1},
          {<<0>>, 1, 1, 0},
          {<<0>>, 1, 1, 5},
          {<<0>>, 65_536, 65_536, 1}
        ] do
      path = Path.join(tmp_dir, "invalid.#{format}")
      assert {:error, _} = Nif.to_binary(format, data, height, width, channels)
      assert {:error, _} = Nif.write_file(path, format, data, height, width, channels)
      refute File.exists?(path)
    end
  end

  @tag :tmp_dir
  test "public encoding operations reject manually constructed invalid images", %{
    tmp_dir: tmp_dir
  } do
    image = %StbImage{data: <<>>, shape: {1, 1, 4}, type: {:u, 8}}
    path = Path.join(tmp_dir, "invalid.png")

    assert_raise ArgumentError, fn -> StbImage.to_binary(image, :png) end
    assert {:error, _} = StbImage.write_file(image, path)
    refute File.exists?(path)
  end
end
