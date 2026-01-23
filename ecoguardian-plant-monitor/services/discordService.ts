export const sendDiscordAlert = async (plantName: string, score: number, issues: string[]) => {
    const webhookUrl = import.meta.env.VITE_DISCORD_WEBHOOK_URL;

    if (!webhookUrl || webhookUrl === 'YOUR_DISCORD_WEBHOOK_URL') {
        console.warn("Discord Webhook URL not configured.");
        return;
    }

    const message = {
        username: "EcoGuardian Bot",
        avatar_url: "https://cdn-icons-png.flaticon.com/512/628/628283.png", // Plant icon
        embeds: [
            {
                title: `🚨 Alerte Santé : ${plantName}`,
                description: `La santé de votre plante est critique !`,
                color: 15158332, // Rouge
                fields: [
                    {
                        name: "Score de Bien-être",
                        value: `${score}%`,
                        inline: true
                    },
                    {
                        name: "Problèmes détectés",
                        value: issues.length > 0 ? issues.join('\n') : "Inconnu",
                        inline: false
                    }
                ],
                footer: {
                    text: "EcoGuardian - Surveillance en temps réel"
                },
                timestamp: new Date().toISOString()
            }
        ]
    };

    try {
        const response = await fetch(webhookUrl, {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
            },
            body: JSON.stringify(message),
        });

        if (!response.ok) {
            console.error("Failed to send Discord alert:", response.statusText);
        } else {
            console.log("Discord alert sent successfully!");
        }
    } catch (error) {
        console.error("Error sending Discord alert:", error);
    }
};
