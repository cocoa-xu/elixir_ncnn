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
      x = trunc(x)
      y = trunc(y)
      w = trunc(w)
      h = trunc(h)
      {:ok, mat} = OpenCV.rectangle(mat, [x, y], [x + w, y + h], [255, 0, 0])
      label_text = " " <> Enum.at(labels(), label_index) <> ": #{Float.round(prob, 2)} "
      IO.puts label_text
      {:ok, {{label_weight, label_height}, baseline}} = OpenCV.gettextsize(label_text, OpenCV.cv_font_hershey_simplex, 0.5, 1)
      y = y - label_height - baseline
      if y < 0, do: y = 0
      if x + label_weight > cols, do: x = cols - label_weight
      x = trunc(x)
      y = trunc(y)
      label_weight = trunc(label_weight)
      label_height = trunc(label_height)
      {:ok, mat} = OpenCV.rectangle(mat, [x, y], [x + label_weight, y + label_height + baseline], [255, 255, 255])
      {:ok, mat} = OpenCV.puttext(mat, label_text, [x, y + label_height],
        OpenCV.cv_font_hershey_simplex, 0.5, [0, 0, 255], thickness: 1)
      translate_pred(rest, mat, cols, threshold)
    else
      translate_pred(rest, mat, cols, threshold)
    end
  end
  def translate_pred([], mat, _, _), do: {:ok, mat}

  def translate_pred(source_mat, pred, threshold \\ 0.5) do
    {:ok, {_, cols, _}} = OpenCV.Mat.shape(source_mat)
    translate_pred(pred, source_mat, cols, threshold)
  end
end
