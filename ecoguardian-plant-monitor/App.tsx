import React, { useState, useEffect, useRef } from 'react';
import { AreaChart, Area, XAxis, YAxis, CartesianGrid, Tooltip, ResponsiveContainer } from 'recharts';
import { PlantData, PlantProfile } from './types';
import { PLANT_PROFILES } from './constants';
import { fetchPlantDataFromInflux, DEFAULT_CONFIG } from './services/influxService';
import { sendDiscordAlert } from './services/discordService';
import { MetricCard } from './components/MetricCard';
import { VirtualPlant } from './components/VirtualPlant';
import { MetricModal } from './components/MetricModal';
import { PlantDetailCard } from './components/PlantDetailCard';
import { ThermometerIcon, DropletsIcon, SunIcon, SproutIcon, ActivityIcon } from './components/Icons';

function App() {
  const [data, setData] = useState<PlantData[]>([]);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);
  const [selectedPlant, setSelectedPlant] = useState<PlantProfile>(PLANT_PROFILES[0]);
  const [lastAlertTime, setLastAlertTime] = useState<number>(0);

  // State for Modal
  const [selectedMetric, setSelectedMetric] = useState<{
    key: keyof PlantData;
    title: string;
    color: string;
    unit: string;
    min?: number;
    max?: number;
  } | null>(null);

  const loadData = async () => {
    try {
      setError(null);
      const results = await fetchPlantDataFromInflux(DEFAULT_CONFIG);
      // Removed "No data found" error throw to allow empty state if just no data yet
      setData(results);
    } catch (err: any) {
      console.error("Erreur InfluxDB:", err);
      setError(err instanceof Error ? err.message : "Erreur inconnue lors de la récupération des données");
    } finally {
      setLoading(false);
    }
  };

  useEffect(() => {
    loadData();
    const interval = setInterval(loadData, 10000);
    return () => clearInterval(interval);
  }, []);

  const current = data.length > 0 ? data[data.length - 1] : {
    temperature: 0, humidite: 0, humidite_sol: 0, luminosite: 0
  };

  /* Calcul du Bien-être */
  let wellnessScore = 0;
  if (data.length > 0) {
    let score = 0;
    const { temperature, humidite, humidite_sol, luminosite } = selectedPlant.needs;

    // Check Temperature
    if (current.temperature >= temperature.min && current.temperature <= temperature.max) score += 25;
    // Check Humidity
    if (current.humidite >= humidite.min && current.humidite <= humidite.max) score += 25;
    // Check Soil Humidity
    if (current.humidite_sol >= humidite_sol.min && current.humidite_sol <= humidite_sol.max) score += 25;
    // Check Light
    if (current.luminosite >= luminosite.min && current.luminosite <= luminosite.max) score += 25;

    wellnessScore = score;
  }

  // --- Gestion des Alertes Discord (dans un useEffect pour éviter les effets de bord au rendu) ---
  useEffect(() => {
    if (data.length > 0) {
      const { temperature, humidite, humidite_sol, luminosite } = selectedPlant.needs;

      if (wellnessScore <= 50) {
        const now = Date.now();
        const COOLDOWN = 60 * 60 * 1000; // 1 heure

        if (now - lastAlertTime > COOLDOWN) {
          const issues: string[] = [];
          if (current.temperature < temperature.min) issues.push(`❄️ Température trop basse (${current.temperature}°C)`);
          if (current.temperature > temperature.max) issues.push(`🔥 Température trop haute (${current.temperature}°C)`);
          if (current.humidite < humidite.min) issues.push(`desert️ Air trop sec (${current.humidite}%)`);
          if (current.humidite > humidite.max) issues.push(`💧 Air trop humide (${current.humidite}%)`);
          if (current.humidite_sol < humidite_sol.min) issues.push(`🌵 Sol trop sec (${current.humidite_sol}%)`);
          if (current.humidite_sol > humidite_sol.max) issues.push(`🌊 Sol trop arrosé (${current.humidite_sol}%)`);
          if (current.luminosite < luminosite.min) issues.push(`🌑 Manque de lumière (${current.luminosite} lux)`);
          if (current.luminosite > luminosite.max) issues.push(`☀️ Trop de lumière (${current.luminosite} lux)`);

          sendDiscordAlert(selectedPlant.name, wellnessScore, issues);
          setLastAlertTime(now);
        }
      }
    }
  }, [wellnessScore, selectedPlant, lastAlertTime, data, current]);

  return (
    <div className="min-h-screen pb-12 bg-[#f0fdf4]">
      {/* Header avec Gradient et Plante Virtuelle */}
      <div className="bg-gradient-to-r from-emerald-600 to-teal-500 pb-24 pt-8 px-6 shadow-lg relative overflow-hidden">
        {/* Cercles décoratifs en arrière-plan */}
        <div className="absolute top-0 right-0 -mr-20 -mt-20 w-96 h-96 rounded-full bg-white opacity-5"></div>
        <div className="absolute bottom-0 left-0 -ml-20 -mb-20 w-72 h-72 rounded-full bg-white opacity-5"></div>

        <div className="max-w-7xl mx-auto flex flex-col md:flex-row items-center justify-between relative z-10">

          <div className="text-center md:text-left mb-6 md:mb-0">
            <h1 className="text-4xl font-bold text-white flex items-center justify-center md:justify-start gap-3 mb-2">
              <SproutIcon /> EcoGuardian
            </h1>
            <p className="text-emerald-100 opacity-90 text-lg mb-4">
              Tableau de bord intelligent pour vos plantes
            </p>

            {/* Plant Selector & Wellness Score */}
            <div className="flex flex-col sm:flex-row gap-4 items-start sm:items-center">
              <div className="inline-flex items-center bg-white/10 backdrop-blur-md rounded-lg p-2 border border-white/20">
                <span className="text-white text-sm font-medium mr-2 px-2">Plante surveillée :</span>
                <select
                  value={selectedPlant.id}
                  onChange={(e) => {
                    const plant = PLANT_PROFILES.find(p => p.id === e.target.value);
                    if (plant) {
                      setSelectedPlant(plant);
                      setLastAlertTime(0); // Reset alert timer on plant change
                    }
                  }}
                  className="bg-white/20 text-white border-none rounded-md px-3 py-1 focus:ring-2 focus:ring-white/50 cursor-pointer hover:bg-white/30 transition-colors"
                  style={{ color: 'white', backgroundColor: 'transparent' }}
                >
                  {PLANT_PROFILES.map(plant => (
                    <option key={plant.id} value={plant.id} className="text-slate-800">
                      {plant.name}
                    </option>
                  ))}
                </select>
              </div>

              {/* Wellness Score Badge */}
              <div className={`inline-flex items-center px-4 py-2 rounded-lg backdrop-blur-md border border-white/20 text-white font-bold shadow-lg
                ${wellnessScore >= 75 ? 'bg-emerald-500/80' : wellnessScore >= 50 ? 'bg-amber-500/80' : 'bg-rose-500/80'}`}>
                <ActivityIcon />
                <span className="ml-2">Bien-être : {wellnessScore}%</span>
              </div>

              <button
                onClick={() => sendDiscordAlert(selectedPlant.name, 42, ["🧪 Test Manuel", "🔔 Ceci est une alerte de test"])}
                className="bg-indigo-500 hover:bg-indigo-600 text-white px-3 py-1 rounded-md text-sm font-medium transition-colors shadow-lg"
              >
                🔔 Test Discord
              </button>
            </div>

            {error && (
              <div className="mt-4 bg-red-500/80 text-white px-4 py-2 rounded-lg text-sm font-medium border border-white/20">
                ⚠️ Erreur : {error}
              </div>
            )}
          </div>

          {/* Plante Virtuelle */}
          <div className="bg-white/10 backdrop-blur-md rounded-full p-6 border border-white/20 shadow-2xl transform hover:scale-105 transition-transform">
            <VirtualPlant
              wellnessScore={wellnessScore}
            />
            <div className="text-center mt-2">
              <span className="text-white/80 text-sm font-medium px-3 py-1 bg-black/20 rounded-full">
                {selectedPlant.name}
              </span>
            </div>
          </div>
        </div>
      </div>

      <div className="max-w-7xl mx-auto px-6 -mt-16 relative z-20">

        {/* Détail de la Plante Sélectionnée */}
        <PlantDetailCard plant={selectedPlant} />

        {/* Grille des cartes (CLIQUABLES) */}
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-6 mb-8">
          <MetricCard
            title="Température"
            value={current.temperature}
            unit="°C"
            icon={<ThermometerIcon />}
            colorClass="text-rose-500"
            bgColorClass="bg-rose-50"
            onClick={() => setSelectedMetric({
              key: 'temperature',
              title: 'Température',
              color: '#e74c3c',
              unit: '°C',
              min: selectedPlant.needs.temperature.min,
              max: selectedPlant.needs.temperature.max
            })}
            min={selectedPlant.needs.temperature.min}
            max={selectedPlant.needs.temperature.max}
          />
          <MetricCard
            title="Humidité Air"
            value={current.humidite}
            unit="%"
            icon={<DropletsIcon />}
            colorClass="text-sky-500"
            bgColorClass="bg-sky-50"
            onClick={() => setSelectedMetric({
              key: 'humidite',
              title: 'Humidité Air',
              color: '#3498db',
              unit: '%',
              min: selectedPlant.needs.humidite.min,
              max: selectedPlant.needs.humidite.max
            })}
            min={selectedPlant.needs.humidite.min}
            max={selectedPlant.needs.humidite.max}
          />
          <MetricCard
            title="Humidité Sol"
            value={current.humidite_sol}
            unit="%"
            subValue={`Raw: ${current.humidite_sol_raw ?? 'N/A'}`}
            icon={<ActivityIcon />}
            colorClass="text-emerald-500"
            bgColorClass="bg-emerald-50"
            onClick={() => setSelectedMetric({
              key: 'humidite_sol',
              title: 'Humidité Sol',
              color: '#10b981',
              unit: '%',
              min: selectedPlant.needs.humidite_sol.min,
              max: selectedPlant.needs.humidite_sol.max
            })}
            min={selectedPlant.needs.humidite_sol.min}
            max={selectedPlant.needs.humidite_sol.max}
          />
          <MetricCard
            title="Luminosité"
            value={current.luminosite}
            unit="%"
            subValue={`Raw: ${current.luminosite_raw ?? 0} Lumen`}
            icon={<SunIcon />}
            colorClass="text-amber-500"
            bgColorClass="bg-amber-50"
            onClick={() => setSelectedMetric({
              key: 'luminosite',
              title: 'Luminosité',
              color: '#f59e0b',
              unit: '%',
              min: selectedPlant.needs.luminosite.min,
              max: selectedPlant.needs.luminosite.max
            })}
            min={selectedPlant.needs.luminosite.min}
            max={selectedPlant.needs.luminosite.max}
          />
        </div>

        {/* Graphique Global (Réduit pour laisser la place à l'interaction individuelle) */}
        <div className="bg-white rounded-2xl shadow-md p-6 border border-slate-100">
          <div className="flex items-center justify-between mb-6">
            <h2 className="text-xl font-bold text-slate-800 flex items-center gap-2">
              <ActivityIcon /> Vue d'ensemble (Dernière heure)
            </h2>
            <div className="flex gap-2">
              <span className="flex items-center text-xs font-medium text-rose-500"><span className="w-2 h-2 rounded-full bg-rose-500 mr-1"></span> Temp</span>
              <span className="flex items-center text-xs font-medium text-sky-500"><span className="w-2 h-2 rounded-full bg-sky-500 mr-1"></span> Air</span>
              <span className="flex items-center text-xs font-medium text-emerald-500"><span className="w-2 h-2 rounded-full bg-emerald-500 mr-1"></span> Sol</span>
            </div>
          </div>

          <div className="h-[300px] w-full">
            {loading ? (
              <div className="h-full flex items-center justify-center text-slate-400">
                Chargement des données...
              </div>
            ) : (
              <ResponsiveContainer width="100%" height="100%">
                <AreaChart data={data} margin={{ top: 10, right: 10, left: 0, bottom: 0 }}>
                  <CartesianGrid strokeDasharray="3 3" vertical={false} stroke="#f1f5f9" />
                  <XAxis dataKey="time" tick={{ fill: '#94a3b8', fontSize: 12 }} tickLine={false} axisLine={false} minTickGap={30} />
                  <YAxis tick={{ fill: '#94a3b8', fontSize: 12 }} tickLine={false} axisLine={false} />
                  <Tooltip contentStyle={{ borderRadius: '12px', border: 'none', boxShadow: '0 4px 6px -1px rgb(0 0 0 / 0.1)' }} />
                  <Area type="monotone" dataKey="temperature" stroke="#e74c3c" strokeWidth={2} fill="none" />
                  <Area type="monotone" dataKey="humidite" stroke="#3498db" strokeWidth={2} fill="none" />
                  <Area type="monotone" dataKey="humidite_sol" stroke="#10b981" strokeWidth={2} fill="url(#colorSol)" fillOpacity={0.2} />
                  <defs>
                    <linearGradient id="colorSol" x1="0" y1="0" x2="0" y2="1">
                      <stop offset="5%" stopColor="#10b981" stopOpacity={0.4} />
                      <stop offset="95%" stopColor="#10b981" stopOpacity={0} />
                    </linearGradient>
                  </defs>
                </AreaChart>
              </ResponsiveContainer>
            )}
          </div>
        </div>
      </div>

      {/* Modale Graphique Détaillé */}
      {
        selectedMetric && (
          <MetricModal
            isOpen={!!selectedMetric}
            onClose={() => setSelectedMetric(null)}
            title={selectedMetric.title}
            dataKey={selectedMetric.key}
            color={selectedMetric.color}
            unit={selectedMetric.unit}
            min={selectedMetric.min}
            max={selectedMetric.max}
            data={data}
          />
        )
      }
    </div >
  );
}

export default App;