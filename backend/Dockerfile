# Use an official Python runtime as the parent image
FROM python:3.11.5-slim

# Set the working directory in the container
WORKDIR /usr/src/app

# Copy the current directory contents into the container at /usr/src/app
COPY . .

# Install any needed packages specified in requirements.txt
RUN pip install fastapi uvicorn scikit-learn==1.1.3  numpy pydantic pandas

# Make port 80 available to the world outside this container
EXPOSE 1000

# Run the app when the container launches
CMD ["uvicorn", "api:app", "--host", "0.0.0.0", "--port", "1000", "--reload"]
