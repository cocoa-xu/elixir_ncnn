defmodule Ncnn.Models.MobileNet do
  alias Ncnn.Models.MobileNet, as: MobileNet

  @enforce_keys [:version, :net]
  defstruct [:version, :net]

  @doc """
  Load MobileNet model
  """
  def load(version, opts \\ [])
  def load(:v1, opts) do
    param = opts[:param] || "./mobilenet_ssd_voc_ncnn.param"
    model = opts[:model] || "./mobilenet_ssd_voc_ncnn.bin"
    load_v1(param, model)
  end
  def load(:v2, opts) do
    param = opts[:param] || "./mobilenetv2_ssdlite_voc.param"
    model = opts[:model] || "./mobilenetv2_ssdlite_voc.bin"
    load_v2(param, model)
  end
  def load(:v3, opts) do
    param = opts[:param] || "./mobilenetv3_ssdlite_voc.param"
    model = opts[:model] || "./mobilenetv3_ssdlite_voc.bin"
    load_v3(param, model)
  end

  @doc """
  Load MobileNetSSD param and model

  model is converted from [chuanqi305/MobileNet-SSD](https://github.com/chuanqi305/MobileNet-SSD)
  and can be downloaded from https://drive.google.com/open?id=0ByaKLD9QaPtucWk0Y0dha1VVY0U
  the ncnn model https://github.com/nihui/ncnn-assets/tree/master/models
  """
  def load_v1(
        param \\ "./mobilenet_ssd_voc_ncnn.param",
        model \\ "./mobilenet_ssd_voc_ncnn.bin") do
    {:ok, net} = Ncnn.Net.net()
    {:ok, net} = Ncnn.Net.load_param(net, param)
    {:ok, net} = Ncnn.Net.load_model(net, model)
    %MobileNet{
      version: :v1,
      net: net
    }
  end

  @doc """
  Load MobileNetV2SSDLite param and model

  original pretrained model from [chuanqi305/MobileNetv2-SSDLite](https://github.com/chuanqi305/MobileNetv2-SSDLite)

  https://github.com/chuanqi305/MobileNetv2-SSDLite/blob/master/ssdlite/voc/deploy.prototxt

  the ncnn model https://github.com/nihui/ncnn-assets/tree/master/models
  """
  def load_v2(
        param \\ "./mobilenetv2_ssdlite_voc.param",
        model \\ "./mobilenetv2_ssdlite_voc.bin") do
    {:ok, net} = :ncnn_nif.mobile_net_v2_ssdlite_load([param: param, model: model])
    %MobileNet{
      version: :v2,
      net: net
    }
  end

  @doc """
  Load MobileNetV3SSDLite param and model

  converted ncnn model from https://github.com/ujsyehao/mobilenetv3-ssd
  """
  def load_v3(
        param \\ "./mobilenetv3_ssdlite_voc.param",
        model \\ "./mobilenetv3_ssdlite_voc.bin") do
    {:ok, net} = Ncnn.Net.net()
    {:ok, net} = Ncnn.Net.load_param(net, param)
    {:ok, net} = Ncnn.Net.load_model(net, model)
    %MobileNet{
      version: :v3,
      net: net
    }
  end

  @doc """
  MobileNet classification labels
  """
  def labels, do: [
                "background", "aeroplane", "bicycle", "bird", "boat",
                "bottle", "bus", "car", "cat", "chair", "cow",
                "diningtable", "dog", "horse", "motorbike", "person",
                "pottedplant", "sheep", "sofa", "train", "tvmonitor"]

  def predict(self=%MobileNet{version: version, net: net}, data, cols, rows)
  when is_binary(data) and is_integer(cols) and is_integer(rows) do
    :ncnn_nif.mobile_net_forward([net: net, version: version, data: data, cols: cols, rows: rows])
  end

  def translate_pred([{label_index, prob, {{x, y}, {w, h}}}|rest], mat, cols, threshold) do
    if prob > threshold do
      x = trunc(x)
      y = trunc(y)
      w = trunc(w)
      h = trunc(h)
      {:ok, mat} = OpenCV.rectangle(mat, [x, y], [x + w, y + h], [255, 0, 0])
      label_text = " " <> Enum.at(labels(), label_index) <> ": #{Float.round(prob, 2)} "
      # IO.puts label_text
      {:ok, {{label_weight, label_height}, baseline}} = OpenCV.gettextsize(label_text, OpenCV.cv_font_hershey_simplex, 0.5, 1)
      y = y - label_height - baseline
      y = if y < 0, do: 0, else: y
      x = if x + label_weight > cols, do: x = cols - label_weight, else: x
      x = trunc(x)
      y = trunc(y)
      label_weight = trunc(label_weight)
      label_height = trunc(label_height)
      {:ok, mat} = OpenCV.rectangle(mat, [x, y], [x + label_weight, y + label_height + baseline], [255, 255, 255])
      {:ok, mat} = OpenCV.puttext(mat, label_text, [x, y + label_height], OpenCV.cv_font_hershey_simplex, 0.5, [0, 0, 255])
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
