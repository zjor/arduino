class ApiClient {
  constructor() {
    this.baseUrl = import.meta.env.VITE_BASE_URL
  }

  async getRGBState() {
    const url = `${this.baseUrl}/api/rgb`
    const response = await fetch(url)
    return await response.json()
  }

  async setRGBState({r, g, b}) {
    const url = `${this.baseUrl}/api/rgb`
    const response = await fetch(url, {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json'
      },
      body: JSON.stringify({r, g, b})
    })
    return await response.json()
  }

}

export const apiClient = new ApiClient()
