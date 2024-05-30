import os
import cv2
import matplotlib.pyplot as plt
import numpy as np


source_file = "../codingtest.mov"

source = cv2.VideoCapture(source_file)

if not source.isOpened():
  print("Error opening video stream or file")

# bpm 61.25 over this roi
# y = [150,235]
# x = [400, 540]

# bpm 61.25 over this roi
# y = [150,435]
# x = [400, 540]

# bpm 61.25 over this roi
y = [185,400]
x = [345, 580]

# opencv window for displaying image
win_name = "heart_rate_monitor"
cv2.namedWindow(win_name, cv2.WINDOW_NORMAL)
alive = True

# parameters for displaying annotation text
fontScale = 1.5
fontFace = cv2.FONT_HERSHEY_PLAIN
fontColor = (0, 255, 0)
fontThickness = 2

avg_g = [] # averages of green channel
window_size = 120 # rolling window size for fft
fps_rate = 30


while alive:
  ret, frame = source.read()
  if not ret:
    break

  # cropped roi of the image (used for bpm detection)
  cropped_roi = frame[ y[0]: y[1], x[0]:x[1]]

  avg = cv2.mean(cropped_roi)
  avg_g.append(avg[1]) # take second channel from "BGR"
  bpm = None

  # calculate bpm
  if len(avg_g) > window_size:
    windowed_signal = avg_g[-window_size:]
    # can remove dc component by subtracting mean. but the test result did not show any effect on bpm calculation
    signal = windowed_signal # - np.mean(windowed_signal) # removes dc component by substracting mean
    ft = np.fft.rfft(signal) # uses rfft(real input fft) instead of fft
    power  = np.log10(np.abs(ft) ** 2)
    frequencies = np.fft.rfftfreq(len(signal), d = 1./fps_rate)
    ## From Jim bpm detected by Presage is 80-180bpm, 1.33 to 3 hz
    ## well trained athlete can have as low as 40bpm, 0.66 hz
    idx_to_clear =  ~((frequencies > 0.66) & (frequencies < 3))
    power[idx_to_clear] = 0
    highest_peak = np.argmax(power)
    bpm = frequencies[highest_peak]*60

  if bpm:  
    text = f"Avg red: {avg[2]:.3f} green: {avg[1]:.3f} blue: {avg[0]:.3f} bpm: {bpm:.3f}"
  else:
    text = f"Avg red: {avg[2]:.3f} green: {avg[1]:.3f} blue: {avg[0]:.3f} bpm: ..calculating in {window_size - len(avg_g)}.."
  cv2.rectangle(frame, (x[0], y[0]), (x[1], y[1]), (0, 255, 0), thickness=5, lineType=cv2.LINE_8)
  cv2.putText(frame, text, (50, 100), fontFace, fontScale, fontColor, fontThickness, cv2.LINE_AA);
  cv2.imshow(win_name,frame)
  key = cv2.waitKey(1)
  if key == ord("Q") or key == ord("q") or key == 27:
    alive = False

# release resources
source.release()
cv2.destroyWindow(win_name)

# plot average of green channels
plt.figure(figsize=[11, 8])
time_values = np.arange(1, len(avg_g) + 1) / 30.0
plt.subplot(311); plt.plot(time_values, avg_g)
plt.xlabel("time (s)")
plt.ylabel("Average green (0 - 255)")

# plot fft of green channel across the entire signal
ft = np.fft.rfft(avg_g)
power  = np.log10(np.abs(ft) ** 2)
frequencies = np.fft.rfftfreq(len(avg_g), d = 1./fps_rate)

plt.subplot(312); plt.plot(frequencies, power)
plt.xlabel("frequencies (Hz)")
plt.ylabel("PSD")

## plot relevant portion of fft with annotation for bpm
## From Jim W. bpm detected by Presage is 80-180bpm, 1.33 to 3 hz
## well trained athlete can have as low as 40bpm, 0.66 hz
idx_to_clear =  ~((frequencies > 0.66) & (frequencies < 3))
power[idx_to_clear] = 0

plt.subplot(313); plt.plot(frequencies, power)
highest_peak = np.argmax(power)
bpm = frequencies[highest_peak]*60

bpm_text= f"BPM: {bpm:.2f}"
plt.text(frequencies[highest_peak], power[highest_peak], bpm_text, 
         fontsize=12, ha='left', va='center')
plt.xlabel("frequencies (Hz)")
plt.ylabel("PSD")
print(bpm_text)

plt.tight_layout()
plt.show()
