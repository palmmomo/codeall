# Use an official Python runtime as the parent image
FROM python:3.11.5

# Set the working directory in the container
WORKDIR /usr/src/app

# Copy the contents of the invertor-agents folder into the container
COPY . .

RUN pip install fastapi uvicorn scikit-learn==1.1.3 pandas==1.5.3 xgboost numpy pyodbc pydantic pandas
RUN  pip install --upgrade pip
# RUN apt-get update && apt-get install -y unixODBC
RUN apt-get update && apt-get install -y unixodbc
# Install prerequisites
RUN apt-get install -y gpg apt-transport-https curl

# Register the Microsoft ODBC Driver 18 for SQL Server
RUN curl https://packages.microsoft.com/keys/microsoft.asc | apt-key add -
RUN curl https://packages.microsoft.com/config/debian/10/prod.list > /etc/apt/sources.list.d/mssql-release.list

# Install the driver
RUN apt-get update
RUN ACCEPT_EULA=Y apt-get install -y msodbcsql18


EXPOSE 2023 

# Run the script when the container launches
CMD ["uvicorn", "api:app", "--host", "0.0.0.0", "--port", "2023", "--reload"]