export interface PlantData {
  time: string;
  originalDate: Date; // Keep original date for sorting if needed
  temperature: number;
  humidite: number;
  humidite_sol: number;
  humidite_sol_raw?: number;
  luminosite: number;
  luminosite_raw?: number;
}

export enum HealthStatus {
  Excellent = "Excellent",
  Bon = "Bon",
  Moyen = "Moyen",
  Critique = "Critique",
  Inconnu = "Inconnu"
}

export interface InfluxConfig {
  url: string;
  token: string;
  org: string;
  bucket: string;
}

export interface Range {
  min: number;
  max: number;
}

export interface PlantProfile {
  id: string;
  name: string;
  description: string;
  needs: {
    temperature: Range;
    humidite: Range;
    humidite_sol: Range;
    luminosite: Range;
  }
}