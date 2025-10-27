from setuptools import setup, find_packages

setup(
    name="mev-shield-client",
    version="0.1.0",
    description="Python client for MEV Shield",
    packages=find_packages(),
    install_requires=[
        "requests>=2.25.0",
        "websockets>=10.0",
    ],
    python_requires=">=3.7",
)
