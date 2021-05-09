# Librerías necesarias
import tensorflow as tf
import numpy as np
import matplotlib.pyplot as plt
import math
from tensorflow.keras import layers

# ----- DATOS DE ENTRENAMIENTO -----
# Entradas
X = np.array([[0,0,0],
     [0,0,1],
     [0,1,0],
     [0,1,1],
    [1,0,0],
    [1,0,1],
    [1,1,0],
    [1,1,1]])

# Salidas
Y = np.array([[0,1,0,1],
              [1,0,0,0],
              [1,0,0,0],
              [1,0,0,0],
              [0,0,1,0],
              [0,0,1,0],
              [0,0,1,0],
              [0,0,1,0]])
              
# Red neuronal
model = tf.keras.Sequential()
model.add(layers.Dense(4, activation="sigmoid", input_shape=(1,3)))
model.add(layers.Dense(3, activation="sigmoid"))
model.add(layers.Dense(4))
model.summary()

# Compilación del modelo
model.compile(optimizer="rmsprop", loss="mse", metrics=['mse'])

# Entrenamiento del modelo
history = model.fit(X,
                    Y,
                    epochs=8000,
                    verbose=False)
                    
# Gráfica del error
loss = history.history["loss"]
epochs = range(1, len(loss)+1)
plt.plot(epochs, loss, 'bo', label="Training loss")

# Se extraen los parametros entrenados de la red neuronal y se les da formato
# para copiarlos directamente al codigo de Arduino
s = "// ****************************** RED NEURONAL ******************************\n\n"
for i in range(0,len(model.layers)):
  matriz_pesos = model.layers[i].get_weights()[0]
  bias = model.layers[i].get_weights()[1]
  s += f"#define RENSWLAYER{i} {matriz_pesos.shape[0]}\n"
  s += f"#define COLSWLAYER{i} {matriz_pesos.shape[1]}\n"
  s += f"BLA::Matrix<RENSWLAYER{i},COLSWLAYER{i}> weights_layer{i} = " + str(matriz_pesos.tolist()).replace("[[", "{").replace("[", "").replace("],", ",\n").replace("]]", "}") +";\n\n"
  s += f"#define TAMBLAYER{i} {bias.shape[0]}\n"
  s += f"BLA::Matrix<1,TAMBLAYER{i}> bias_layer{i} = " + str(bias.tolist()).replace("[", "{").replace("]", "}") + ";\n\n\n"

print(s)
                    
