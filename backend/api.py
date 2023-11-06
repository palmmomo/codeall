from fastapi import FastAPI, HTTPException
from pydantic import BaseModel
import pandas as pd
from sklearn.ensemble import RandomForestRegressor
from sklearn.model_selection import train_test_split
from datetime import datetime
from fastapi.middleware.cors import CORSMiddleware


class DateInput(BaseModel):
    year: int
    month: int
    day: int

# Initialize FastAPI app
app = FastAPI()

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Load the dataset
df = pd.read_csv('co2_levels_thailand_regions.csv')

# Preprocess the date column
df['Date'] = pd.to_datetime(df['Date'])
df['Year'] = df['Date'].dt.year
df['Month'] = df['Date'].dt.month
df['Day'] = df['Date'].dt.day

# Prepare the features (X) and target (y)
regions = ['ภาคเหนือ', 'ภาคกลาง', 'ภาคตะวันออกเฉียงเหนือ', 'ภาคใต้']
X = df[['Year', 'Month', 'Day'] + [f'{region}_Factories' for region in regions]]
y = df[[f'{region}_CO2' for region in regions]]

# Split the data
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

# Initialize the model
model = RandomForestRegressor(n_estimators=100, random_state=42)

# Train the model
model.fit(X_train, y_train)

# FastAPI route for making predictions
@app.post("/predict/")
async def predict(input: DateInput):
    # Create a DataFrame for the input date
    date_df = pd.DataFrame({
        'Year': [input.year],
        'Month': [input.month],
        'Day': [input.day],
        # Assuming average factories data is known and constant for the prediction
        'ภาคเหนือ_Factories': [30],
        'ภาคกลาง_Factories': [70],
        'ภาคตะวันออกเฉียงเหนือ_Factories': [40],
        'ภาคใต้_Factories': [25]
    })

    # Make the prediction
    predicted_co2 = model.predict(date_df)
    # Convert the prediction to a Python list and return it
    return predicted_co2.tolist()

# Run the FastAPI server with uvicorn
# Execute the following command in the terminal:
# uvicorn script_name:app --reload
