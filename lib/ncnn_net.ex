defmodule Ncnn.Net do
  def net do
    :ncnn_nif.net_new_net()
  end

  def load_param(self, param_path)
  when is_binary(param_path) do
    :ncnn_nif.net_load_param([net: self, param_path: param_path])
  end

  def load_model(self, model_path)
  when is_binary(model_path) do
    :ncnn_nif.net_load_model([net: self, model_path: model_path])
  end

  def forward(self, image_path) when is_binary(image_path) do
    {:ok, mat} = OpenCV.imread(image_path)
    {:ok, {rows, cols, _}} = OpenCV.Mat.shape(mat)
    {:ok, bin_data} = OpenCV.Mat.to_binary(mat)
    {:ok, pred} = forward(self, bin_data, cols, rows)
    {:ok, mat, pred}
  end

  def forward(_self, data, cols, rows)
  when is_binary(data) and is_integer(cols) and is_integer(rows) do
    raise "not implemented"
  end
end
