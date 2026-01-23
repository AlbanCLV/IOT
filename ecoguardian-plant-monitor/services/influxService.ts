import { InfluxDB } from '@influxdata/influxdb-client';
import { PlantData, InfluxConfig } from '../types';

// Configuration par défaut (à adapter selon votre setup Docker)
export const DEFAULT_CONFIG: InfluxConfig = {
  url: import.meta.env.VITE_INFLUX_URL,
  token: import.meta.env.VITE_INFLUX_TOKEN,
  org: import.meta.env.VITE_INFLUX_ORG,
  bucket: import.meta.env.VITE_INFLUX_BUCKET
};

// Fonction pour générer des données simulées si la base de données n'est pas connectée
export const generateMockData = (count: number = 20): PlantData[] => {
  const data: PlantData[] = [];
  const now = new Date();

  for (let i = count; i > 0; i--) {
    const time = new Date(now.getTime() - i * 5 * 60000); // Toutes les 5 minutes

    // Simulation de cycles naturels
    const baseTemp = 22;
    const tempNoise = Math.random() * 2 - 1;

    const baseHum = 50;
    const humNoise = Math.random() * 5 - 2.5;

    const baseLux = 500;
    // Luminosité plus forte "récemment"
    const luxNoise = Math.max(0, baseLux + (Math.random() * 200 - 100));

    data.push({
      time: time.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' }),
      originalDate: time,
      temperature: parseFloat((baseTemp + tempNoise).toFixed(1)),
      humidite: parseFloat((baseHum + humNoise).toFixed(1)),
      humidite_sol: parseFloat((60 + Math.random() * 2).toFixed(1)), // Sol reste stable
      luminosite: Math.round(luxNoise)
    });
  }
  return data;
};

export const fetchPlantDataFromInflux = async (config: InfluxConfig): Promise<PlantData[]> => {
  const influxDB = new InfluxDB({ url: config.url, token: config.token });
  const queryApi = influxDB.getQueryApi(config.org);

  const fluxQuery = `
    from(bucket: "${config.bucket}")
      |> range(start: -24h)
      |> filter(fn: (r) => r["_measurement"] == "monitor_plante")
      |> pivot(rowKey:["_time"], columnKey: ["_field"], valueColumn: "_value")
      |> sort(columns: ["_time"])
  `;

  const results: PlantData[] = [];

  return new Promise((resolve, reject) => {
    queryApi.queryRows(fluxQuery, {
      next(row, tableMeta) {
        const o = tableMeta.toObject(row);

        // Soil Calibration
        const rawSoil = o.humidite_sol != null ? Number(o.humidite_sol) : 0;
        let soilPercent = ((rawSoil - 300) / (950 - 300)) * 100;
        soilPercent = Math.max(0, Math.min(100, soilPercent));

        // Light Calibration
        const rawLight = o.luminosite != null ? Number(o.luminosite) : 0;
        let lightPercent = (rawLight / 1023) * 100;
        lightPercent = Math.max(0, Math.min(100, lightPercent));

        results.push({
          time: new Date(o._time).toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' }),
          originalDate: new Date(o._time),
          temperature: o.temperature != null ? parseFloat(Number(o.temperature).toFixed(1)) : 0,
          humidite: o.humidite != null ? parseFloat(Number(o.humidite).toFixed(1)) : 0,
          humidite_sol: parseFloat(soilPercent.toFixed(1)),
          humidite_sol_raw: Math.round(rawSoil),
          luminosite: Math.round(lightPercent),
          luminosite_raw: Math.round(rawLight)
        });
      },
      error(error) {
        // Suppressed console.error to allow handling in App.tsx without noise
        reject(error);
      },
      complete() {
        resolve(results);
      },
    });
  });
};