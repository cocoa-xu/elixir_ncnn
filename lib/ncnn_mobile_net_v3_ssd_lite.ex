defmodule Ncnn.MobileNetV3SsdLite do
  def load(param \\ "./mobilenetv3_ssdlite_voc.param", model \\ "./mobilenetv3_ssdlite_voc.bin") do
    {:ok, net} = Ncnn.Net.net()
    {:ok, net} = Ncnn.Net.load_param(net, param)
    Ncnn.Net.load_model(net, model)
  end

  def labels, do: [
                "background", "aeroplane", "bicycle", "bird", "boat",
                "bottle", "bus", "car", "cat", "chair", "cow",
                "diningtable", "dog", "horse", "motorbike", "person",
                "pottedplant", "sheep", "sofa", "train", "tvmonitor"]

  def predict(self, image_path) do
    Ncnn.Net.forward(self, image_path)
  end

  def predict(self, bin_data, cols, rows) do
    Ncnn.Net.forward(self, bin_data, cols, rows)
  end

  def translate_pred([{label_index, prob, {{x, y}, {w, h}}}|rest], mat, cols, threshold) do
    if prob > threshold do
#      {:ok, mat} = OpenCV.rectangle(mat, [x, y, w, h], [255, 0, 0])
      IO.puts Enum.at(labels(), label_index) <> ": #{Float.round(prob, 2)}"
      # todo: OpenCV.gettextsize seems always return an invalid value
#      {:ok, {{label_height, label_weight}, baseline}} = OpenCV.gettextsize(label_text, OpenCV.cv_font_hershey_simplex, 0.5, 1)
#      y = y - label_height - baseline
#      if y < 0, do: y = 0
#      if x + label_weight > cols, do: x = cols - label_weight
#      {:ok, mat} = OpenCV.rectangle(mat, [x, y], [x + label_weight, y + label_height + baseline], [255, 255, 255])
#      {:ok, mat} = OpenCV.puttext(mat, label_text, [0, 0], OpenCV.cv_font_hershey_simplex, 0.5, [0, 0, 0])
      translate_pred(rest, mat, cols, threshold)
    else
      translate_pred(rest, mat, cols, threshold)
    end
  end
  def translate_pred([], mat, _, _), do: mat

  def translate_pred(source_mat, pred, threshold \\ 0.5) do
    {:ok, {_, cols, _}} = OpenCV.Mat.shape(source_mat)
    translate_pred(pred, source_mat, cols, threshold)
  end
end
